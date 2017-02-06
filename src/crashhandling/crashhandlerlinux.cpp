#ifdef CRASHHANDLING_LINUX

#include "crashhandlerlinux.h"
#include "crashhandlingprovider.h"
#include <algorithm>
#include <cstring>
#include <errno.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>

#define HANDLE_EINTR(x) ({ \
  __typeof__(x) eintr_wrapper_result; \
  do { \
    eintr_wrapper_result = (x); \
  } while (eintr_wrapper_result == -1 && errno == EINTR); \
  eintr_wrapper_result; \
})

const std::array<int, 6> CrashHandlerLinux::m_handledSignals = { SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP };

CrashHandlerLinux::RawMemPage::RawMemPage(const size_t bytes) :
  mem(nullptr),
  m_pageSize(getpagesize()),
  m_NPages((bytes % m_pageSize == 0) ? (bytes / m_pageSize) : (bytes / m_pageSize) + 1)
{
  mem = mmap(nullptr, mapSize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

CrashHandlerLinux::RawMemPage::~RawMemPage()
{
  if (mem != nullptr)
    munmap(mem, mapSize());
}

size_t CrashHandlerLinux::RawMemPage::mapSize() const
{

  return m_NPages * m_pageSize;
}

CrashHandlerLinux::CrashHandlerLinux(const std::__cxx11::string &miniDumpPath) :
  CrashHandlerBase(miniDumpPath),
  m_mainThreadId(syscall(__NR_gettid))
{
  m_pipeDes[0] = 1;
  m_pipeDes[1] = -1;
}

CrashHandlerLinux::~CrashHandlerLinux()
{
  for (const int signum : m_handledSignals)
    restoreHandler(signum);

  if (restoreOriginalStack() == false)
    _exit(1);

  free(m_alternateStack.ss_sp);
}

constexpr size_t CrashHandlerLinux::alternateStackSize()
{
  return (16384 > SIGSTKSZ) ? 16384 : SIGSTKSZ;
}

int CrashHandlerLinux::clonedProcessFunc(void *param)
{
  /* Wait till the parent lets us ptrace it */
  CrashHandlerLinux *me = CrashHandlingProvider<CrashHandlerLinux>::handler();
  ChildParam *chParam = static_cast<ChildParam *>(param);

  me->waitForParent();

}

void CrashHandlerLinux::crashHandlerFunc(int signum, siginfo_t *siginfo, void *vuctx)
{
  CrashHandlerLinux *me = CrashHandlingProvider<CrashHandlerLinux>::handler();

  pthread_mutex_lock(&me->m_handlerMutex);

  me->handleSignal(siginfo, vuctx);

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

/* This may run in a compromised context */
void CrashHandlerLinux::generateMiniDump()
{
  RawMemPage raw(16384);
  if (raw.mem == nullptr)
    return;

  uint8_t *stack = reinterpret_cast<uint8_t *>(raw.mem);
  stack += 16384; /* Stack grows downwards on x86 */
  /* Zeroize the top of the stack */
  for (size_t offset = 0; offset <= 16; offset++)
    *(stack - 16 + offset) = 0;

  /* Use the pipe to block the cloned process */
  if (pipe(m_pipeDes) < 0) {
    m_pipeDes[0] = -1;
    m_pipeDes[1] = -1;
  }

  const pid_t child = clone(clonedProcessFunc, stack, CLONE_FILES | CLONE_FS | CLONE_UNTRACED, nullptr, nullptr, nullptr, nullptr);
  if (child < 0) {
    close(m_pipeDes[0]);
    close(m_pipeDes[1]);
    return;
  }

  prctl(PR_SET_PTRACER, child, 0, 0);
  runChild();

  int status;
  const int ret = HANDLE_EINTR(waitpid(child, &status, __WALL));

  close(m_pipeDes[0]);
  close(m_pipeDes[1]);

  const bool haveDump = (ret != -1) && WIFEXITED(status) && (WEXITSTATUS(status) == 0);
  if (haveDump)
    writeDump();
}

/* This may run in a compromised context */
void CrashHandlerLinux::handleSignal(siginfo_t *siginfo, void *vuctx)
{
  struct ucontext *uctx = static_cast<struct ucontext *>(vuctx);
  const bool signalTrusted = siginfo->si_code > 0;
  const bool signalPidTrusted = siginfo->si_code == SI_USER || siginfo->si_code == SI_TKILL;

  if (signalTrusted || (signalPidTrusted && siginfo->si_pid == getpid()))
    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);

  if (uctx->uc_mcontext.fpregs)
    memcpy(&m_crashCtx.fpuState, uctx->uc_mcontext.fpregs, sizeof(struct _libc_fpstate));

  m_crashCtx.exceptionThreadId = syscall(__NR_gettid);

  generateMiniDump();

}

bool CrashHandlerLinux::install()
{
  struct sigaction crashAction;

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
  if (restoreOriginalStack() == false)
    _exit(1); /* TODO: Report the failure to restore the original stack to the user */
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

bool CrashHandlerLinux::restoreOriginalStack()
{
  stack_t currentStack;
  bool altStackDisabled;

  if (sigaltstack(nullptr, &currentStack) < 1) /* Get info about the currently used stack */
    return false;

  if (m_alternateStack.ss_sp == currentStack.ss_sp) { /* Are we on the alternate stack? */
    if (m_originalStack.ss_sp != nullptr) /* Do we have the original stack? */
      altStackDisabled = sigaltstack(&m_originalStack, nullptr) == 0;
    else {
      /* No pointer to the original stack so just disable the current one */
      stack_t stackDisabler;

      stackDisabler.ss_flags = SS_DISABLE;
      altStackDisabled = sigaltstack(&stackDisabler, nullptr) == 0;
    }
  } else
    altStackDisabled = true;

  return altStackDisabled;
}

void CrashHandlerLinux::runChild()
{
  static const char proceed = 'a';

  HANDLE_EINTR(write(m_pipeDes[1], &proceed, sizeof(char)));
}

void CrashHandlerLinux::uninstall()
{
}

void CrashHandlerLinux::waitForParent()
{
  char recv;
  HANDLE_EINTR(read(m_pipeDes[0], &recv, sizeof(char)));
}

void CrashHandlerLinux::writeDump()
{

}

#endif // CRASHHANDLING_LINUX
