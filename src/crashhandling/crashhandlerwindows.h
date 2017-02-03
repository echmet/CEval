/**********************************************************************************
 *   The crash handling logic used in this code is                                *
 *   based on the implementation used by the Google Breakpad project              *
 *   Visit project's page https://chromium.googlesource.com/breakpad/breakpad     *
 *   for further information.                                                     *
 **********************************************************************************/

#ifndef CRASHHANDLERWINDOWS_H
#define CRASHHANDLERWINDOWS_H

#ifdef CRASHHANDLING_WIN32

#include "crashhandlerbase.h"
#include <windows.h>

class CrashHandlerWindows : public CrashHandlerBase {
public:
  enum class CrashType {
    UNHANDLED_EXCEPTION,
    PURE_VIRTUAL_CALL,
    INVALID_PARAMETER,
    HEAP_CORRUPTION
  };

  explicit CrashHandlerWindows(const std::string &miniDumpPath);
  virtual ~CrashHandlerWindows() override;
  virtual const std::string & crashInfo() const override;
  void handleCrash(const CrashType crash, LPEXCEPTION_POINTERS exptrs = nullptr);
  void handleCrashThreadExecutor();
  virtual bool mainThreadCrashed() const override;
  virtual bool install() override;
  virtual void uninstall() override;
  virtual void proceedToKill() const override;
  virtual void waitForKill() override;

private:
  HANDLE m_handlerStartSemaphore;
  HANDLE m_handlerFinishSemaphore;
  HANDLE m_handlerThread;
  HANDLE m_handlerThreadStarted;
  bool m_shutdownHandlerThread;
  CRITICAL_SECTION m_handlerCriticalSection;
  LPTSTR m_tstrMiniDumpPath;

  HANDLE m_proceedToKillSemaphore;

  LPTOP_LEVEL_EXCEPTION_FILTER m_originalUnhandledExceptionHandler;
  _purecall_handler m_originalPureVirtualHandler;
  _invalid_parameter_handler m_originalInvalidParameterHandler;

  DWORD m_mainThreadId;
  LPEXCEPTION_POINTERS m_currentExPtrs;
  DWORD m_exceptionThreadId;
  CrashType m_crashType;
  std::string m_crashInfo;

};

#endif // CRASHHANDLING_WIN32

#endif // CRASHHANDLERWINDOWS_H
