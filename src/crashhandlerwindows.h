#ifndef CRASHHANDLERWINDOWS_H
#define CRASHHANDLERWINDOWS_H

//#ifdef Q_OS_WIN

#include "abstractcrashhandler.h"
#include <windows.h>

class CrashHandlerWindows : public AbstractCrashHandler {
public:
  CrashHandlerWindows();
  ~CrashHandlerWindows();
  void handleCrash(LPEXCEPTION_POINTERS exptrs);
  void handleCrashThreadExecutor();
  virtual bool install() override;
  virtual void uninstall() override;

private:
  HANDLE m_handlerStartSemaphore;
  HANDLE m_handlerFinishSemaphore;
  HANDLE m_handlerThread;
  HANDLE m_handlerThreadStarted;
  bool m_shutdownHandlerThread;

  LPTOP_LEVEL_EXCEPTION_FILTER m_originalExceptionFilter;
  _purecall_handler m_originalPureVirtualHandler;

  LPEXCEPTION_POINTERS m_currentExPtrs;
};

//#endif // Q_OS_WIN

#endif // CRASHHANDLERWINDOWS_H
