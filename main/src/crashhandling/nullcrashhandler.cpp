#include "nullcrashhandler.h"

NullCrashHandler::NullCrashHandler(const std::string &) :
  CrashHandlerBase(""),
  m_nullCrashInfo("")
{
}

const std::string & NullCrashHandler::crashInfo() const
{
  return m_nullCrashInfo;
}

bool NullCrashHandler::install()
{
  return true;
}

bool NullCrashHandler::mainThreadCrashed() const
{
  return true;
}

void NullCrashHandler::uninstall()
{
  return;
}
