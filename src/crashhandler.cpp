#include "crashhandler.h"
#include <QApplication>
#include <QEventLoop>
#include <QList>
#include <QMessageBox>
#include <signal.h>
#include "gui/crashhandlerdialog.h"
#include <QThread>
#include <QDebug>

#define WAIT_LOOP { QEventLoop __loop; __loop.exec(); }
#define WAIT_IF_THREADED { if (QThread::currentThread() != qApp->thread()) { QEventLoop __loop; __loop.exec(); } }

CrashHandler::CrashEvent::CrashEvent(const QString &what) : QEvent(static_cast<QEvent::Type>(QEvent::User + 1)),
  what(what)
{
}

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
  std::set_terminate(&CrashHandler::runawayExceptionHandler);
  std::set_unexpected(&CrashHandler::runawayExceptionHandler);
#endif
}

CrashHandler::~CrashHandler()
{
#ifdef USE_CRASHHANDLER
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

void CrashHandler::die()
{
  std::abort();
}

bool CrashHandler::event(QEvent *ev)
{
  if (ev->type() != QEvent::User + 1)
    return false;

  CrashEvent *cev = dynamic_cast<CrashEvent *>(ev);
  if (cev == nullptr)
    return false;

  emit s_me->emergency();
  handleCrash(cev->what);
  die();

  return true;
}

void CrashHandler::handleCrash(const QString &what)
{
  CrashHandlerDialog dlg;
  QString backtrace = createBacktrace(what);

  dlg.setBacktrace(backtrace);
  dlg.exec();
}

void CrashHandler::install()
{
#ifdef USE_CRASHHANDLER
  if (s_me != nullptr)
    return;

  try {
    s_me = new CrashHandler();
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Crash handler"), tr("Unable to install crash handler"));
    return;
  }

  qDebug() << "Crash handler installed";

#endif
}

void CrashHandler::runawayExceptionHandler()
{
  signal(SIGABRT, nullptr);
  signal(SIGSEGV, nullptr);

  qDebug() << "Unhandled exception caught, crashing controllably";
  QCoreApplication::postEvent(s_me, new CrashEvent("Unhandled exception"));
  WAIT_LOOP;
}

void CrashHandler::sigabrtHandler(int c)
{
  Q_UNUSED(c);

  signal(SIGABRT, nullptr);
  signal(SIGSEGV, nullptr);

  qDebug() << "Unhandled exception caught, crashing controllably";
  QCoreApplication::postEvent(s_me, new CrashEvent("Aborted!"));
  WAIT_IF_THREADED;
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

  qDebug() << "Unhandled exception caught, crashing controllably";
  QCoreApplication::postEvent(s_me, new CrashEvent("Segmentation fault!"));
  WAIT_IF_THREADED;
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


