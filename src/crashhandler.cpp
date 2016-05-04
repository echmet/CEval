#include "crashhandler.h"
#include <QApplication>
#include <QList>
#include <QMessageBox>
#include <signal.h>
#include "gui/crashhandlerdialog.h"

#include <QDebug>

#ifdef Q_OS_WIN
    #include "stacktrace_win.h"
#elif defined Q_OS_UNIX
    #include "stacktrace.h"
#endif // Q_OS_

CrashHandler *CrashHandler::s_me = nullptr;

CrashHandler::CrashHandler(QObject *parent) :
  QObject(parent)
{
#ifdef USE_CRASHHANDLER
  signal(SIGABRT, &CrashHandler::sigabrtHandler);
  signal(SIGINT, &CrashHandler::sigintHandler);
  signal(SIGSEGV, &CrashHandler::sigsegvHandler);
  signal(SIGTERM, &CrashHandler::sigtermHandler);

  qDebug() << "Crash handler installed";
#endif
}

CrashHandler::~CrashHandler()
{
#ifndef DEBUG
  signal(SIGABRT, nullptr);
  signal(SIGINT, nullptr);
  signal(SIGSEGV, nullptr);
  signal(SIGTERM, nullptr);
#endif
}

QString CrashHandler::createBacktrace(const QString &what)
{
  QString backtrace;
#ifdef Q_OS_WIN
  backtrace = straceWin::getBacktrace();
#elif defined Q_OS_UNIX
  backtrace = getBacktrace();
#endif // Q_OS_

  return what + QString("\n") + backtrace;
}

CrashHandler *CrashHandler::pointer()
{
  return s_me;
}

void CrashHandler::showBacktrace(const QString &what)
{
  CrashHandlerDialog dlg;
  QString backtrace = createBacktrace(what);

  dlg.setBacktrace(backtrace);
  dlg.exec();
}

void CrashHandler::install()
{
#ifndef DEBUG
  if (s_me != nullptr)
    return;

  try {
    s_me = new CrashHandler();
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Crash handler"), tr("Unable to install crash handler"));
    return;
  }

#endif
}

void CrashHandler::sigabrtHandler(int c)
{
  Q_UNUSED(c);

  signal(SIGABRT, nullptr);
  signal(SIGSEGV, nullptr);
  qDebug() << "SIGABRT caught, crashing controllably";

  emit s_me->emergency();
  showBacktrace("Aborted!");

  raise(SIGABRT);
}

void CrashHandler::sigintHandler(int c)
{
  Q_UNUSED(c)

  signal(SIGINT, nullptr);
  qDebug() << "SIGINT caught, exiting cleanly";
  qApp->exit();

}

void CrashHandler::sigsegvHandler(int c)
{
  Q_UNUSED(c)

  signal(SIGABRT, nullptr);
  signal(SIGSEGV, nullptr);
  qDebug() << "SIGSEGV caught, crashing controllably";

  emit s_me->emergency();
  showBacktrace("Segmentation fault!");

  raise(SIGSEGV);
}

void CrashHandler::sigtermHandler(int c)
{
  Q_UNUSED(c)

  signal(SIGTERM, nullptr);
  qDebug() << "SIGTERM caught, exiting cleanly";
  qApp->exit();
}

void CrashHandler::uninstall()
{
  if (s_me != nullptr) {
    delete s_me;
    s_me = nullptr;
  }
}


