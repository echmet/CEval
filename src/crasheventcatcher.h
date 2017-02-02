#ifndef CRASHEVENTCATCHER_H
#define CRASHEVENTCATCHER_H

#include <QEvent>
#include <QObject>

class CrashEventCatcher : public QObject
{
  Q_OBJECT
public:
  CrashEventCatcher();

protected:
  bool eventFilter(QObject *watched, QEvent *event);

private:
  const QEvent::Type m_crashEventId;

signals:
  void emergency();

};

#endif // CRASHEVENTCATCHER_H
