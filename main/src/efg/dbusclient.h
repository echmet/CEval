#ifndef DBUSCLIENT_H
#define DBUSCLIENT_H

#ifdef ENABLE_IPC_INTERFACE_DBUS

#include <QObject>
#include "ipcclient.h"

class QDBusInterface;

namespace efg {

class DBusClient : public IPCClient
{
public:
  explicit DBusClient();
  virtual ~DBusClient();

  virtual bool loadData(NativeDataVec &ndVec, const QString &formatTag) override;
  virtual bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats) override;

private:
  QDBusInterface *m_iface;
};

} // namespace efg

#endif // ENABLE_IPC_INTERFACE_DBUS

#endif // DBUSCLIENT_H
