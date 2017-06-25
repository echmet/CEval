#include "dbusclient.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QTime>
#include "../../../efgloader/core/common/ipcinterface.h"

#include <QDebug>

namespace efg {

DBusClient::DBusClient() : IPCClient()
{
  DBusMetyTypesRegistrator::registerAll();
  QDBusConnection conn = QDBusConnection::sessionBus();

  QTime waitFrom = QTime::currentTime();
  while (!conn.interface()->isServiceRegistered(DBUS_SERVICE_NAME)) {
    QTime now = QTime::currentTime();

    if (waitFrom.msecsTo(now) > 5000)
      throw std::runtime_error("D-Bus service is not available");
  }

  m_iface = new QDBusInterface(DBUS_SERVICE_NAME, DBUS_OBJECT_PATH, "");
  if (!m_iface->isValid()) {
    QDBusError err = m_iface->lastError();
    delete m_iface;
    m_iface = nullptr;
    throw std::runtime_error(QString("%1: %2").arg(err.name()).arg(err.message()).toUtf8().data());
  }
}

bool DBusClient::loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath)
{
  QDBusReply<IPCDBusDataPack> reply;

  if (hintPath == "")
    reply = m_iface->call("loadData", formatTag);
  else
    reply = m_iface->call("loadDataHint", formatTag, hintPath);

  if (!reply.isValid()) {
    qDebug() << reply.error().name() << reply.error().message();
    return false;
  }

  const IPCDBusDataPack &pack = reply.value();

  qDebug() << pack.success << pack.error;

  for (const IPCDBusData &data : pack.data) {
    qDebug() << data.name << data.path << data.xDescription << data.yDescription << data.xUnit << data.yUnit;

    QVector<QPointF> nativeDatapoints = [](const QVector<IPCDBusDatapoint> &dpts) {
      QVector<QPointF> ndpts;

      ndpts.resize(dpts.size());
      for (const IPCDBusDatapoint &dp : dpts)
        ndpts.append(QPointF(dp.x, dp.y));

      return ndpts;
    }(data.datapoints);

    auto ndd = std::shared_ptr<EFGData>(new EFGData(nativeDatapoints,
                                                    data.xDescription, data.xUnit,
                                                    data.yDescription, data.yUnit));
    ndVec.append(NativeData(ndd, data.path, data.name));
  }

  return true;
}

bool DBusClient::supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats)
{
  QDBusReply<IPCDBusSupportedFileFormatVec> reply = m_iface->call("supportedFileFormats");

  if (!reply.isValid())
    return false;

  for (const auto &sff : reply.value())
    supportedFormats.push_back(EFGSupportedFileFormat(sff.tag, sff.description, sff.tag));

  return true;
}

DBusClient::~DBusClient()
{
  delete m_iface;
}

} //namespace efg
