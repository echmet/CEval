#ifdef CRASHHANDLING_WIN32

#include "crashhandlerwindows.h"
#include "crashhandlerwindows_stacktrace.h"
#include "crashhandlingprovider.h"

#ifndef DBG_PRINTEXCEPTION_WIDE_C
#define DBG_PRINTEXCEPTION_WIDE_C 0x4001000A
#endif

DWORD WINAPI handlerThreadFunc(LPVOID param)
{
  CrashHandlerWindows *me = static_cast<CrashHandlerWindows *>(param);

  me->handleCrashThreadExecutor();

  return 0;
}

LONG WINAPI unhandledExceptionHandler(LPEXCEPTION_POINTERS exptrs)
{
  CrashHandlerWindows *me = CrashHandlingProvider<CrashHandlerWindows>::handler();
  const DWORD code = exptrs->ExceptionRecord->ExceptionCode;

  switch (code) {
  case EXCEPTION_BREAKPOINT:
  case EXCEPTION_SINGLE_STEP:
  case DBG_PRINTEXCEPTION_C:
  case DBG_PRINTEXCEPTION_WIDE_C:
    return EXCEPTION_CONTINUE_EXECUTION;
  }

  /* NOTE: We are unlikely to catch an exception of type 0xC0000374 - Heap Corruption
   * as the CRT library handles this internally and does not allow us to continue execution
   * in any meaningful way anyway. */
  me->handleCrash(code == 0xC0000374 ?
                    CrashHandlerWindows::CrashType::HEAP_CORRUPTION :
                    CrashHandlerWindows::CrashType::UNHANDLED_EXCEPTION,
                  exptrs);

  return me->mainThreadCrashed() ? EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER;
}

void __cdecl invalidParameterHandler(const TCHAR *expression, const TCHAR *function, const TCHAR *file, unsigned int line, uintptr_t pReserved)
{
  (void)expression; (void)function; (void)file; (void)line; (void)pReserved;

  CrashHandlerWindows *me = CrashHandlingProvider<CrashHandlerWindows>::handler();

  me->handleCrash(CrashHandlerWindows::CrashType::INVALID_PARAMETER);
}

void __cdecl pureVirtualHandler()
{
  CrashHandlerWindows *me = CrashHandlingProvider<CrashHandlerWindows>::handler();

  me->handleCrash(CrashHandlerWindows::CrashType::PURE_VIRTUAL_CALL);
}

CrashHandlerWindows::CrashHandlerWindows(const std::string &miniDumpPath) :
  CrashHandlerBase(miniDumpPath),
  m_shutdownHandlerThread(false),
  m_mainThreadId(GetCurrentThreadId())
{
#if defined _UNICODE || defined UNICODE
  const size_t length = MultiByteToWideChar(CP_ACP, 0, miniDumpPath.c_str(), -1, NULL, 0);
  m_tstrMiniDumpPath = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, length * sizeof(TCHAR));
  if (m_tstrMiniDumpPath != NULL)
    MultiByteToWideChar(CP_ACP, 0, miniDumpPath.c_str(), -1, m_tstrMiniDumpPath, length);
#else
  const size_t length = (miniDumpPath.length() + 1) * sizeof(TCHAR);
  m_tstrMiniDumpPath = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, length);
  if (m_tstrMiniDumpPath != NULL)
    memcpy_s(m_tstrMiniDumpPath, length, miniDumpPath.c_str(), length);
#endif
}

CrashHandlerWindows::~CrashHandlerWindows()
{
  HeapFree(GetProcessHeap(), 0, m_tstrMiniDumpPath);
}

const std::string & CrashHandlerWindows::crashInfo() const
{
  return m_crashInfo;
}

/* This is what all exception handlers end up calling */
void CrashHandlerWindows::handleCrash(const CrashType crash, LPEXCEPTION_POINTERS exptrs)
{
  EnterCriticalSection(&m_handlerCriticalSection);

  m_currentExPtrs = exptrs;
  m_exceptionThreadId = GetCurrentThreadId();
  m_crashType = crash;

  ReleaseSemaphore(m_handlerStartSemaphore, 1, NULL);
  WaitForSingleObject(m_handlerFinishSemaphore, INFINITE);

  LeaveCriticalSection(&m_handlerCriticalSection);
}

/* Keep in mind that this is called from the handler thread */
void CrashHandlerWindows::handleCrashThreadExecutor()
{
  std::ostringstream logStream;
  std::string backtrace;

  ReleaseSemaphore(m_handlerThreadStarted, 1, NULL);

  while (true) {
    if (WaitForSingleObject(m_handlerStartSemaphore, INFINITE) == WAIT_OBJECT_0) {
      if (m_shutdownHandlerThread)
        return;

      switch (m_crashType) {
      case CrashType::INVALID_PARAMETER:
        logStream << "Invalid parameter\n";
        break;
      case CrashType::PURE_VIRTUAL_CALL:
        logStream << "Pure virual function call\n";
        break;
      case CrashType::UNHANDLED_EXCEPTION:
        logStream << "Unhandled exception 0x" << std::setw(8) << std::setfill('0') << std::hex << m_currentExPtrs->ExceptionRecord->ExceptionCode << "\n";
        break;
      default:
        ReleaseSemaphore(m_handlerFinishSemaphore, 1, NULL);
        return;
        break;
      }

      backtrace += straceWin::getBacktrace(logStream, m_exceptionThreadId, m_currentExPtrs->ContextRecord);
      m_crashInfo = backtrace.c_str();

      /* Write crash minidump */

      HANDLE hMiniDumpFile = NULL;
      if (m_tstrMiniDumpPath != NULL)
        hMiniDumpFile = CreateFile(m_tstrMiniDumpPath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

      if (hMiniDumpFile != NULL) {
        MINIDUMP_EXCEPTION_INFORMATION dump_ex_info;
        HANDLE hProcess = GetCurrentProcess();

        dump_ex_info.ThreadId = m_exceptionThreadId;
        dump_ex_info.ExceptionPointers = m_currentExPtrs;
        dump_ex_info.ClientPointers = FALSE;

        MiniDumpWriteDump(hProcess, GetProcessId(hProcess), hMiniDumpFile, MiniDumpNormal, &dump_ex_info, NULL, NULL);

        CloseHandle(hMiniDumpFile);
      }

      finalize();

      ReleaseSemaphore(m_handlerFinishSemaphore, 1, NULL);
    }
  }
}

bool CrashHandlerWindows::install()
{
  InitializeCriticalSection(&m_handlerCriticalSection);

  m_proceedToKillSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_proceedToKillSemaphore == NULL)
    return false;

  m_handlerStartSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerStartSemaphore == NULL) {
    CloseHandle(m_proceedToKillSemaphore);
    return false;
  }

  m_handlerFinishSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerFinishSemaphore == NULL)
    goto errout_1;


  m_handlerThreadStarted = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerThreadStarted == NULL)
    goto errout_2;

  m_handlerThread = CreateThread(NULL,
                                 64 * 1024,
                                 handlerThreadFunc,
                                 this,
                                 0,
                                 NULL);

  if (m_handlerThread == NULL)
    goto errout_3;

  WaitForSingleObject(m_handlerThreadStarted, INFINITE);
  CloseHandle(m_handlerThreadStarted);

  m_originalUnhandledExceptionHandler = SetUnhandledExceptionFilter(unhandledExceptionHandler);
  m_originalPureVirtualHandler = _set_purecall_handler(pureVirtualHandler);
  m_originalInvalidParameterHandler = _set_invalid_parameter_handler(invalidParameterHandler);

  return true;

errout_3:
  CloseHandle(m_handlerThreadStarted);
errout_2:
  CloseHandle(m_handlerFinishSemaphore);
errout_1:
  CloseHandle(m_handlerStartSemaphore);

  return false;
}

bool CrashHandlerWindows::mainThreadCrashed() const
{
  return m_mainThreadId == m_exceptionThreadId;
}

void CrashHandlerWindows::uninstall()
{
  m_shutdownHandlerThread = true;
  _set_invalid_parameter_handler(m_originalInvalidParameterHandler);
  _set_purecall_handler(m_originalPureVirtualHandler);
  SetUnhandledExceptionFilter(m_originalUnhandledExceptionHandler);

  ReleaseSemaphore(m_handlerStartSemaphore, 1, NULL);
  WaitForSingleObject(m_handlerThread, 10000);

  CloseHandle(m_handlerThread);
  CloseHandle(m_handlerFinishSemaphore);
  CloseHandle(m_handlerStartSemaphore);
  CloseHandle(m_proceedToKillSemaphore);
}

void CrashHandlerWindows::proceedToKill() const
{
  ReleaseSemaphore(m_proceedToKillSemaphore, 1, NULL);
}

void CrashHandlerWindows::waitForKill()
{
  WaitForSingleObject(m_proceedToKillSemaphore, INFINITE);
}

#endif // CRASHHANDLING_WIN32
