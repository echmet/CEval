#ifndef CRASHHANDLERLINUX_H
#define CRASHHANDLERLINUX_H

#ifdef CRASHHANDLING_LINUX

#include "crashhandlerbase.h"
#include <array>
#include <pthread.h>
#include <signal.h>

class CrashHandlerLinux : public CrashHandlerBase
{
public:
  explicit CrashHandlerLinux(const std::string &miniDumpPath);
  virtual ~CrashHandlerLinux() override;
  virtual const std::string & crashInfo() const override;
  virtual bool install() override;
  virtual bool mainThreadCrashed() const override;
  virtual void uninstall() override;

private:
  void handleSignal(siginfo_t *siginfo, void *uctx);
  void restoreHandler(const int signum);
  static constexpr size_t alternateStackSize();
  static void crashHandlerFunc(int signum, siginfo_t *siginfo, void *uctx);
  static bool retrigger(const int signum);

  std::string m_crashInfo;

  pthread_mutex_t m_handlerMutex;
  stack_t m_originalStack;
  stack_t m_alternateStack;
  std::array<struct sigaction, 6> m_originalHandlers;
  std::array<struct sigaction, 6> m_crashHandlers;

  const pid_t m_mainThreadId;

  static const std::array<int, 6> m_handledSignals;

};

#endif // CRASHHANDLING_LINUX

#endif // CRASHHANDLERLINUX_H
