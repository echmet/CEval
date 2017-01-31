//#ifdef Q_OS_WIN

#include "crashhandlerwindows.h"
#include "crashhandlerprovider.h"
#include <QDebug>

#ifndef DBG_PRINTEXCEPTION_WIDE_C
#define DBG_PRINTEXCEPTION_WIDE_C 0x4001000A
#endif // DBG_PRINTEXCEPTION_WIDE_C

DWORD WINAPI handlerThreadFunc(LPVOID param)
{
  CrashHandlerWindows *me = static_cast<CrashHandlerWindows *>(param);

  me->handleCrashThreadExecutor();

  return 0;
}

LONG WINAPI exceptionFilter(LPEXCEPTION_POINTERS exptrs)
{
  CrashHandlerWindows *me = static_cast<CrashHandlerWindows *>(CrashHandlerProvider::handler());
  const DWORD exCode = exptrs->ExceptionRecord->ExceptionCode;
  const bool isDebugException = (exCode == EXCEPTION_BREAKPOINT) ||
                                (exCode == EXCEPTION_SINGLE_STEP) ||
                                (exCode == DBG_PRINTEXCEPTION_C) ||
                                (exCode == DBG_PRINTEXCEPTION_WIDE_C);

  if (isDebugException)
    return EXCEPTION_CONTINUE_EXECUTION;

  /* Handle the crash */
  /* TODO: Make this actually thread-safe */
  me->handleCrash(exptrs);

  return EXCEPTION_EXECUTE_HANDLER;
}

void __cdecl pureVirtualHandler()
{
  CrashHandlerWindows *me = static_cast<CrashHandlerWindows *>(CrashHandlerProvider::handler());
  me->handleCrash(nullptr);
}

CrashHandlerWindows::CrashHandlerWindows() :
  m_shutdownHandlerThread(false)
{
}

CrashHandlerWindows::~CrashHandlerWindows()
{

}

/* This is what all exception handlers end up calling */
void CrashHandlerWindows::handleCrash(LPEXCEPTION_POINTERS exptrs)
{
  m_currentExPtrs = exptrs;
  ReleaseSemaphore(m_handlerStartSemaphore, 1, NULL);
  WaitForSingleObject(m_handlerFinishSemaphore, INFINITE);
}

/* Keep in mind that this is called from the handler thread */
void CrashHandlerWindows::handleCrashThreadExecutor()
{
  qDebug() << "Crash handler thread launched";

  ReleaseSemaphore(m_handlerThreadStarted, 1, NULL);

  while (true) {
    if (WaitForSingleObject(m_handlerStartSemaphore, INFINITE) == WAIT_OBJECT_0) {
      if (m_shutdownHandlerThread)
        return;

      qDebug() << "Deja Vu, I fall apart!!!";

      ReleaseSemaphore(m_handlerFinishSemaphore, 1, NULL);
    }
  }
}

bool CrashHandlerWindows::install()
{
  m_handlerStartSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerStartSemaphore == NULL)
    return false;

  m_handlerFinishSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerFinishSemaphore == NULL) {
    CloseHandle(m_handlerStartSemaphore);
    return false;
  }

  m_handlerThreadStarted = CreateSemaphore(NULL, 0, 1, NULL);
  if (m_handlerThreadStarted == NULL)
    goto errout_1;

  m_handlerThread = CreateThread(NULL,
                                 64 * 1024,
                                 handlerThreadFunc,
                                 this,
                                 0,
                                 NULL);

  if (m_handlerThread == NULL)
    goto errout_2;

  WaitForSingleObject(m_handlerThreadStarted, INFINITE);
  CloseHandle(m_handlerThreadStarted);

  m_originalExceptionFilter = SetUnhandledExceptionFilter(exceptionFilter);
  m_originalPureVirtualHandler = _set_purecall_handler(pureVirtualHandler);
  qDebug() << "CrashHandlerWindows initialized";

  return true;

errout_2:
  CloseHandle(m_handlerThreadStarted);
errout_1:
  CloseHandle(m_handlerFinishSemaphore);
  CloseHandle(m_handlerStartSemaphore);

  return false;
}

void CrashHandlerWindows::uninstall()
{
  m_shutdownHandlerThread = true;
  _set_purecall_handler(m_originalPureVirtualHandler);
  SetUnhandledExceptionFilter(m_originalExceptionFilter);

  ReleaseSemaphore(m_handlerStartSemaphore, 1, NULL);
  WaitForSingleObject(m_handlerThread, 10000);

  CloseHandle(m_handlerThread);
  CloseHandle(m_handlerFinishSemaphore);
  CloseHandle(m_handlerStartSemaphore);
}


//#endif // Q_OS_WIN
