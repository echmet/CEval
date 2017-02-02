#include "crashhandlerbase.h"

CrashHandlerBase::CrashHandlerBase() :
  m_finalizer(nullptr)
{
}

void CrashHandlerBase::finalize()
{
  if (m_finalizer)
    (*m_finalizer)();
}

void CrashHandlerBase::proceedToKill() const
{
  return;
}

void CrashHandlerBase::setFinalizer(CrashHandlerFinalizerRoot *finalizer)
{
  m_finalizer = finalizer;
}

void CrashHandlerBase::waitForKill()
{
  return;
}
