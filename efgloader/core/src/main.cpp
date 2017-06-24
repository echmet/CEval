#include <QApplication>
#include <iostream>
#include "dataloader.h"

#include "ipcproxy.h"

#ifdef ENABLE_IPC_INTERFACE_DBUS
  #include "dbusipcproxy.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
  #include "localsocketipcproxy.h"

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

int main(int argc, char *argv[])
{
  QApplication a{argc, argv};
  DataLoader loader{};
  IPCProxy *proxy;

  a.setQuitOnLastWindowClosed(false); /* We want our plugins to be able use Qt GUI and not kill us when they close their UI elements */

  try {
    proxy = provisionIPCInterface(&loader);
  } catch (std::exception &ex) {
    std::cerr << "Cannot start CEval data loader: " << ex.what() << std::endl;

    return EXIT_FAILURE;
  }

  int ret = a.exec();

  delete proxy;

  return ret;
}
