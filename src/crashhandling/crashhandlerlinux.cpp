#ifdef CRASHHANDLING_LINUX

#include "crashhandlerlinux.h"
#include "crashhandlingprovider.h"
#include "crashhandlerlinux_stacktrace.h"
#include "rawmemblock.h"
#include <algorithm>
#include <cstring>
#include <errno.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>

#define sys_prctl(param1, param2, param3, param4, param5) syscall(__NR_prctl, param1, param2, param3, param4, param5)
#define sys_sigaltstack(newStack, originalStack) syscall(__NR_sigaltstack, newStack, originalStack)

const std::array<int, 6> CrashHandlerLinux::m_handledSignals = {{ SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP }};

CrashHandlerLinux::CrashHandlerLinux(const std::string &miniDumpPath) :
  CrashHandlerBase(miniDumpPath),
  m_installed(false),
  m_mainThreadId(syscall(__NR_gettid))
{
  m_pipeDes[0] = 1;
  m_pipeDes[1] = -1;
  m_crashInfo.reserve(LinuxStackTracer::MAX_FRAMES * LinuxStackTracer::MAX_LINE_LENGTH + 1);
}

CrashHandlerLinux::~CrashHandlerLinux()
{
  uninstall();
}

constexpr size_t CrashHandlerLinux::alternateStackSize()
{
  return (16384 > SIGSTKSZ) ? 16384 : SIGSTKSZ;
}

void CrashHandlerLinux::crashHandlerFunc(int signum, siginfo_t *siginfo, void *vuctx)
{
  CrashHandlerLinux *me = CrashHandlingProvider<CrashHandlerLinux>::handler();

  pthread_mutex_lock(&me->m_handlerMutex);

  const bool haveDump = me->handleSignal(siginfo, vuctx);

  me->restoreHandler(signum);

  if (haveDump)
    me->finalize();

  pthread_mutex_unlock(&me->m_handlerMutex);

  /* Send the signal to the main thread if necessary */
  if (!me->mainThreadCrashed()) {
    if (syscall(__NR_tgkill, me->m_mainThreadId, getpid(), signum))
      _exit(EXIT_FAILURE);
    return;
  }

  /* Retrigger the signal by hand if it came from a userspace kill() */
  if (siginfo->si_code == SI_USER || signum == SIGABRT) {
    if (retrigger(signum))
      _exit(EXIT_FAILURE);
  }
}

const std::string & CrashHandlerLinux::crashInfo() const
{
  return m_crashInfo;
}

/* This may run in a compromised context */
bool CrashHandlerLinux::generateMiniDump(const int signum)
{
  size_t backtraceLines = 0;
  RawMemBlock<char> backtrace;

  if (!LinuxStackTracer::getBacktrace(backtrace, backtraceLines, signum))
    return false;

  for (size_t idx = 0; idx < backtraceLines; idx++) {
    const char *line = backtrace.mem() + (idx * LinuxStackTracer::MAX_LINE_LENGTH);

    m_crashInfo += std::string(line);
  }

  return true;
}

/* This may run in a compromised context */
bool CrashHandlerLinux::handleSignal(siginfo_t *siginfo, void *vuctx)
{
  ucontext_t *uctx = static_cast<ucontext_t *>(vuctx);
  const bool signalTrusted = siginfo->si_code > 0;
  const bool signalPidTrusted = siginfo->si_code == SI_USER || siginfo->si_code == SI_TKILL;

  if (signalTrusted || (signalPidTrusted && siginfo->si_pid == getpid()))
    sys_prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);

  if (uctx->uc_mcontext.fpregs)
    memcpy(&m_crashCtx.fpuState, uctx->uc_mcontext.fpregs, sizeof(struct _libc_fpstate));

  m_crashCtx.exceptionThreadId = syscall(__NR_gettid);

  return generateMiniDump(siginfo->si_signo);
}

bool CrashHandlerLinux::install()
{
  struct sigaction crashAction;

  if (sem_init(&m_waitForKillSemaphore, 0, 0) == -1)
    return false;

  if (pthread_mutex_init(&m_handlerMutex, nullptr) != 0)
    return false;

  memset(&m_crashCtx, 0, sizeof(CrashContext));
  memset(&m_crashCtx.siginfo, 0, sizeof(siginfo_t));
  memset(&m_crashCtx.uctx, 0, sizeof(ucontext_t));
  memset(&m_crashCtx.fpuState, 0, sizeof(_libc_fpstate));

  memset(&m_originalStack, 0, sizeof(stack_t));
  memset(&m_alternateStack, 0, sizeof(stack_t));

  m_alternateStack.ss_sp = malloc(alternateStackSize()); /* Consider using mmap() instead */
  m_alternateStack.ss_size = alternateStackSize();

  if (sys_sigaltstack(&m_alternateStack, &m_originalStack) < 0)
    goto errout_1;

  /* Store the original crash handlers */
  for (size_t idx = 0; idx < m_handledSignals.size(); idx++) {
    const int signum = m_handledSignals.at(idx);

    if (sigaction(signum, nullptr, &m_originalHandlers[idx]) < 0)
      goto errout_1;
  }

  memset(&crashAction, 0, sizeof(struct sigaction));
  sigemptyset(&crashAction.sa_mask);

  for (const int signum : m_handledSignals)
    sigaddset(&crashAction.sa_mask, signum);

  crashAction.sa_flags = SA_SIGINFO | SA_SIGINFO;
  crashAction.sa_sigaction = crashHandlerFunc;

  for (size_t idx = 0; idx < m_crashHandlers.size(); idx++) {
    const int signum = m_handledSignals.at(idx);

    if (sigaction(signum, &crashAction, nullptr) < 0) {
      /* Something went wrong while wiring up the crash handler.
       * Unwind and error out */
      for (size_t jdx = 0; jdx <= idx; jdx++) {
        const int _signum = m_handledSignals.at(jdx);
        if (sigaction(_signum, &m_originalHandlers[jdx], nullptr) < 0)
          signal(_signum, SIG_DFL); /* Install the default handler if we have failed to restore the original one */
      }
      goto errout_2;
    }
  }

  m_installed = true;

  return true;

errout_2:
  if (restoreOriginalStack() == false)
    _exit(1); /* TODO: Report the failure to restore the original stack to the user */
errout_1:
  free(m_alternateStack.ss_sp);
  return false;

}

bool CrashHandlerLinux::mainThreadCrashed() const
{
  return m_mainThreadId == m_crashCtx.exceptionThreadId;
}

void CrashHandlerLinux::proceedToKill() const
{
  sem_post(const_cast<sem_t *>(&m_waitForKillSemaphore));
}

bool CrashHandlerLinux::retrigger(const int signum)
{
  const pid_t threadId = syscall(__NR_gettid);
  const pid_t processId = getpid();

  return syscall(__NR_tgkill, threadId, processId, signum) == 0;
}

void CrashHandlerLinux::restoreHandler(const int signum)
{
  size_t idx;
  for (idx = 0; idx < m_handledSignals.size(); idx++) {
    if (m_handledSignals.at(idx) == signum)
      break;
  }
  if (idx == m_handledSignals.size())
    return;

  if (sigaction(signum, &m_originalHandlers[idx], nullptr) < 0)
    signal(signum, SIG_DFL);
}

bool CrashHandlerLinux::restoreOriginalStack()
{
  stack_t currentStack;
  bool altStackDisabled;

  if (sys_sigaltstack(nullptr, &currentStack) == -1) /* Get info about the currently used stack */
    return false;

  if (m_alternateStack.ss_sp == currentStack.ss_sp) { /* Are we on the alternate stack? */
    if (m_originalStack.ss_sp != nullptr) /* Do we have the original stack? */
      altStackDisabled = sys_sigaltstack(&m_originalStack, nullptr) == 0;
    else {
      /* No pointer to the original stack so just disable the current one */
      stack_t stackDisabler;

      stackDisabler.ss_flags = SS_DISABLE;
      altStackDisabled = sys_sigaltstack(&stackDisabler, nullptr) == 0;
    }
  } else
    altStackDisabled = true;

  return altStackDisabled;
}

void CrashHandlerLinux::uninstall()
{
  if (!m_installed)
    return;

  for (const int signum : m_handledSignals)
    restoreHandler(signum);

  if (restoreOriginalStack()) {
    free(m_alternateStack.ss_sp);
    memset(&m_alternateStack, 0, sizeof(stack_t));
  }

  m_installed = false;
}

void CrashHandlerLinux::waitForKill()
{
  sem_wait(&m_waitForKillSemaphore);
}


#endif // CRASHHANDLING_LINUX
