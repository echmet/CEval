#ifndef CRASHEVENT_H
#define CRASHEVENT_H

#include "crashhandling/crashhandlerbase.h"

#include <QEvent>

class CrashEvent : public QEvent {
public:
  explicit CrashEvent(const CrashHandlerBase *crashHandler);

  const CrashHandlerBase *crashHandler;

  static QEvent::Type registerMe();

private:
  static QEvent::Type s_myEventType;

};

#endif // CRASHEVENT_H
