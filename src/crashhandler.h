#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QObject>

class CrashHandler : public QObject
{
  Q_OBJECT
public:
  static CrashHandler *pointer();
  static void install();
  static void uninstall();

private:
  CrashHandler(QObject *parent = nullptr);
  ~CrashHandler();

  static QString createBacktrace(const QString &what);
  static void showBacktrace(const QString &what);

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
