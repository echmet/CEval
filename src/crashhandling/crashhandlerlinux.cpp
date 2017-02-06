#ifdef CRASHHANDLING_LINUX

#include "crashhandlerlinux.h"
#include "crashhandlingprovider.h"
#include <algorithm>
#include <cstring>
#include <syscall.h>
#include <unistd.h>

const std::array<int, 6> CrashHandlerLinux::m_handledSignals = { SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP };

CrashHandlerLinux::CrashHandlerLinux(const std::__cxx11::string &miniDumpPath) :
  CrashHandlerBase(miniDumpPath),
  m_mainThreadId(syscall(__NR_gettid))
{
}

CrashHandlerLinux::~CrashHandlerLinux()
{
}

constexpr size_t CrashHandlerLinux::alternateStackSize()
{
  return (16384 > SIGSTKSZ) ? 16384 : SIGSTKSZ;
}


void CrashHandlerLinux::crashHandlerFunc(int signum, siginfo_t *siginfo, void *uctx)
{
  CrashHandlerLinux *me = CrashHandlingProvider<CrashHandlerLinux>::handler();

  pthread_mutex_lock(&me->m_handlerMutex);

  me->handleSignal(siginfo, uctx);

  me->restoreHandler(signum);

  pthread_mutex_unlock(&me->m_handlerMutex);

  /* Retrigger the signal by hand if it came from a userspace kill() */
  if (siginfo->si_code < 0 || signum == SIGABRT) {
    if (retrigger(signum))
      _exit(1);
  }
}

const std::string & CrashHandlerLinux::crashInfo() const
{
  return m_crashInfo;
}

void CrashHandlerLinux::handleSignal(siginfo_t *siginfo, void *uctx)
{

}

bool CrashHandlerLinux::install()
{
  struct sigaction crashAction;

  if (pthread_mutex_init(&m_handlerMutex, nullptr) != 0)
    return false;

  memset(&m_originalStack, 0, sizeof(stack_t));
  memset(&m_alternateStack, 0, sizeof(stack_t));

  m_alternateStack.ss_sp = malloc(alternateStackSize()); /* Consider using mmap() instead */
  m_alternateStack.ss_size = alternateStackSize();

  if (sigaltstack(&m_alternateStack, &m_originalStack) < 0)
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
      /* Something went wrong while wiring up the crash handler
       * Unwind and error out */
      for (size_t jdx = 0; jdx <= idx; jdx++) {
        const int _signum = m_handledSignals.at(jdx);
        if (sigaction(_signum, &m_originalHandlers[jdx], nullptr) < 0)
          signal(_signum, SIG_DFL); /* Install the default handler if we have failed to restore the original one */
      }
      goto errout_2;
    }
  }

  return true;

errout_2:
  sigaltstack(&m_originalStack, nullptr);
errout_1:
  free(m_alternateStack.ss_sp);
  return false;

}

bool CrashHandlerLinux::mainThreadCrashed() const
{
  return true;
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

void CrashHandlerLinux::uninstall()
{
}

#endif // CRASHHANDLING_LINUX
