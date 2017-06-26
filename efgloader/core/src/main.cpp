#include <QApplication>
#include <QProcess>
#include <QTimer>
#include <iostream>
#include "dataloader.h"
#include "ipcproxy.h"

#ifdef ENABLE_IPC_INTERFACE_DBUS
  #include "dbusipcproxy.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
  #include "localsocketipcproxy.h"

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
  #include <errno.h>
  #include <signal.h>
  #include <unistd.h>
#endif

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
void catchSIGTERM()
{
  auto handler = [](int sig) -> void {
    Q_UNUSED(sig);
    QApplication::quit();
  };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    sigaddset(&blocking_mask, SIGTERM);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    sigaction(SIGTERM, &sa, nullptr);
}
#endif

IPCProxy * provisionIPCInterface(DataLoader *loader)
{
#ifdef ENABLE_IPC_INTERFACE_DBUS
  try {
    return new DBusIPCProxy{loader};
  } catch (std::runtime_error &ex) {
    qWarning() << "Cannot register D-Bus iterface (" << ex.what() << "), falling back to local socket";
  }
#endif // ENABLE_IPC_INTERFACE_DBUS
  return new LocalSocketIPCProxy{loader};
}

void watchForMainProcess(QTimer *&timer, const char *pidStr)
{
  bool ok;
  Q_PID pid = QString(pidStr).trimmed().toLongLong(&ok);

  if (!ok)
    return;

  timer = new QTimer{};

  timer->setInterval(1000);
#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
  QObject::connect(timer, &QTimer::timeout, [timer, pid]() {
    int ret = kill(pid, 0);

    if (ret == 0)
      return;
    if (ret == -1 && errno == EPERM)
        return;
    else {
      timer->stop();
      QApplication::quit();
    }
  });
#endif

  timer->start();
}

int main(int argc, char *argv[])
{
  QApplication a{argc, argv};
  DataLoader loader{};
  IPCProxy *proxy;
  QTimer *timer = nullptr;


  if (argc > 1)
    watchForMainProcess(timer, argv[1]);

  a.setQuitOnLastWindowClosed(false); /* We want our plugins to be able use Qt GUI and not kill us when they close their UI elements */

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
  catchSIGTERM();
#endif

  try {
    proxy = provisionIPCInterface(&loader);
  } catch (std::exception &ex) {
    std::cerr << "Cannot start CEval data loader: " << ex.what() << std::endl;

    return EXIT_FAILURE;
  }

  int ret = a.exec();

  if (timer != nullptr && timer->isActive())
    timer->stop();
  delete timer;

  delete proxy;

  return ret;
}
