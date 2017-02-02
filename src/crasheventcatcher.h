#ifndef CRASHEVENTCATCHER_H
#define CRASHEVENTCATCHER_H

#include <QEvent>
#include <QObject>

class CrashEvent;

class CrashEventCatcher : public QObject
{
  Q_OBJECT
public:
  CrashEventCatcher();
  void executeEmergency();

protected:
  bool eventFilter(QObject *watched, QEvent *event);

private:
  void handleCrash(const CrashEvent *event);

  const QEvent::Type m_crashEventId;

signals:
  void emergency();

};

#endif // CRASHEVENTCATCHER_H
