#ifndef CRASHHANDLERLINUX_H
#define CRASHHANDLERLINUX_H

#ifdef CRASHHANDLING_LINUX

#include "crashhandlerbase.h"
#include <array>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <ucontext.h>

class CrashHandlerLinux : public CrashHandlerBase
{
public:
  explicit CrashHandlerLinux(const std::string &miniDumpPath);
  virtual ~CrashHandlerLinux() override;
  virtual const std::string & crashInfo() const override;
  virtual bool install() override;
  virtual void proceedToKill() const override;
  virtual bool mainThreadCrashed() const override;
  virtual void uninstall() override;
  virtual void waitForKill() override;

private:
  class ChildParam {
  public:
    pid_t pid;

  };

  class CrashContext {
  public:
    siginfo_t siginfo;                  /* Currently unused */
    pid_t exceptionThreadId;
    ucontext_t uctx;                    /* Currently unused */
    struct _libc_fpstate fpuState;

  };

  bool generateMiniDump(const int signum);
  bool handleSignal(siginfo_t *siginfo, void *vuctx);
  void restoreHandler(const int signum);
  bool restoreOriginalStack();
  static constexpr size_t alternateStackSize();
  static int clonedProcessFunc(void *param);
  static void crashHandlerFunc(int signum, siginfo_t *siginfo, void *vuctx);
  static bool retrigger(const int signum);

  CrashContext m_crashCtx;
  std::string m_crashInfo;

  pthread_mutex_t m_handlerMutex;
  stack_t m_originalStack;
  stack_t m_alternateStack;
  std::array<struct sigaction, 6> m_originalHandlers;
  std::array<struct sigaction, 6> m_crashHandlers;
  int m_pipeDes[2];
  sem_t m_waitForKillSemaphore;
  bool m_installed;

  const pid_t m_mainThreadId;

  static const std::array<int, 6> m_handledSignals;

};

#endif // CRASHHANDLING_LINUX

#endif // CRASHHANDLERLINUX_H
