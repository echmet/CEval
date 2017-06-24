#ifndef DBUSIPCPROXY_H
#define DBUSIPCPROXY_H

#ifdef ENABLE_IPC_INTERFACE_DBUS

#include "ipcproxy.h"
#include "dbus/dbusinterface.h"

class LoaderAdaptor;

class DBusIPCProxy : public IPCProxy
{
public:
  explicit DBusIPCProxy(DataLoader *loader, QObject *parent = nullptr);
  virtual ~DBusIPCProxy();

private:
  void unprovision();

  DBusInterface *m_interface;
  LoaderAdaptor *m_interfaceAdaptor;

private slots:
  void onSupportedFileFormats(IPCDBusSupportedFileFormatVec &supportedFileFormats);
  void onLoadData(IPCDBusDataPack &pack, const QString &formatTag, const DBusInterface::LoadMode loadMode, const QString &modeParam);
};

#endif // USE_INTERFACE_DBUS

#endif // DBUSIPCPROXY_H
