#include "dbusipcproxy.h"
#include "dbus/dbusinterface.h"
#include "dbus/DBusInterfaceAdaptor.h"
#include "dataloader.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

DBusIPCProxy::DBusIPCProxy(DataLoader *loader, QObject *parent) :
  IPCProxy(loader, parent)
{
  DBusMetyTypesRegistrator::registerAll();
  QDBusConnection connection = QDBusConnection::sessionBus();

  m_interface = new DBusInterface{this};
  m_interfaceAdaptor = new LoaderAdaptor{m_interface};

  if (!connection.interface()->isServiceRegistered(DBUS_SERVICE_NAME)) {
    if (!connection.registerService(DBUS_SERVICE_NAME))
      throw std::runtime_error{"Cannot register D-Bus service"};

    if (!connection.registerObject(DBUS_OBJECT_PATH, m_interface))
      throw std::runtime_error{"Cannot register D-Bus object, bailing out"};
  }

  connect(m_interface, &DBusInterface::loadDataForwarder, this, &DBusIPCProxy::onLoadData);
  connect(m_interface, &DBusInterface::supportedFileFormatsForwarder, this, &DBusIPCProxy::onSupportedFileFormats);
}

DBusIPCProxy::~DBusIPCProxy()
{
  unprovision();

  //delete m_loader;
}

void DBusIPCProxy::onSupportedFileFormats(IPCDBusSupportedFileFormatVec &supportedFileFormats)
{
  const QVector<FileFormatInfo> fileFormatInfoVec = m_loader->supportedFileFormats();

  for (const auto &ffi : fileFormatInfoVec) {
    IPCDBusLoadOptionsVec dbusLoadOpts;

    for (const QString &s : ffi.loadOptions)
      dbusLoadOpts.push_back(s);

    supportedFileFormats.push_back(IPCDBusSupportedFileFormat{ffi.longDescription, ffi.shortDescription, ffi.tag, dbusLoadOpts});
  }
}

void DBusIPCProxy::onLoadData(IPCDBusDataPack &pack, const QString &formatTag, const DBusInterface::LoadMode mode, const QString &modeParam, const int loadOption)
{
  DataLoader::LoadedPack result;

  switch (mode) {
  case DBusInterface::LoadMode::INTERACTIVE:
    result = m_loader->loadData(formatTag, loadOption);
    break;
  case DBusInterface::LoadMode::HINT:
    result = m_loader->loadDataHint(formatTag, modeParam, loadOption);
    break;
  case DBusInterface::LoadMode::FILE:
    result = m_loader->loadDataPath(formatTag, modeParam, loadOption);
    break;
  }

  if (!std::get<1>(result)) {
    pack.success = false;
    pack.error = std::get<2>(result);
  } else {
    pack.success = true;

    for (const Data &d : std::get<0>(result)) {
      IPCDBusData dd;

      dd.name = d.name;
      dd.path = d.path;
      dd.xDescription = d.xDescription;
      dd.yDescription = d.yDescription;
      dd.xUnit = d.xUnit;
      dd.yUnit = d.yUnit;

      for (const auto &point : d.datapoints) {
        IPCDBusDatapoint dp;
        dp.x = std::get<0>(point);
        dp.y = std::get<1>(point);
        dd.datapoints.append(dp);
      }

      pack.data.append(dd);
    }
  }
}

void DBusIPCProxy::unprovision()
{
  QDBusConnection connection = QDBusConnection::sessionBus();

  connection.unregisterObject(DBUS_OBJECT_PATH);
  connection.unregisterService(DBUS_SERVICE_NAME);
}
