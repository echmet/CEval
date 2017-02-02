#include "crashevent.h"

QEvent::Type CrashEvent::s_myEventType = QEvent::None;

CrashEvent::CrashEvent(const CrashHandlerBase *crashHandler) :
  QEvent(registerMe()),
  crashHandler(crashHandler)
{
}

QEvent::Type CrashEvent::registerMe()
{
  if (s_myEventType == QEvent::None) {
    int myTypeId = registerEventType();
    s_myEventType = static_cast<QEvent::Type>(myTypeId);
  }

  return s_myEventType;
}
