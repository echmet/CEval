#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QEvent>
#include <QObject>

class CrashHandler : public QObject
{
  Q_OBJECT
public:
  static CrashHandler *pointer();
  static void install();
  static void uninstall();

private:
  class CrashEvent : public QEvent {
  public:
    CrashEvent(const QString &what);

    const QString what;
  };

  CrashHandler(QObject *parent = nullptr);
  ~CrashHandler();
  bool event(QEvent *ev);
  void die();
  void handleCrash(const QString &what);
  void showBacktrace(const QString &backtrace, const int signalNo);

  static QString createBacktrace(const QString &what);
  static void runawayExceptionHandler();
  static void sigintHandler(int c);
  static void sigtermHandler(int c);
  static void sigsegvHandler(int c);
  static void sigabrtHandler(int c);

  static CrashHandler *s_me;

signals:
  void emergency();
};

#endif // CRASHHANDLER_H
