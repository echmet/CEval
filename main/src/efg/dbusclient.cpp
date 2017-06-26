#include "dbusclient.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QMessageBox>
#include <QThread>
#include "../../../efgloader/core/common/ipcinterface.h"

namespace efg {

const int DBusClient::DEFAULT_TIMEOUT(600000);

DBusClient::DBusClient() : IPCClient(),
  m_iface(nullptr)
{
}

void DBusClient::connectToInterface()
{
  DBusMetyTypesRegistrator::registerAll();
  QDBusConnection conn = QDBusConnection::sessionBus();

  int ctr = 0;
  while (!conn.interface()->isServiceRegistered(DBUS_SERVICE_NAME)) {
    QThread::msleep(500);

    if (ctr++ > 5)
      throw std::runtime_error("D-Bus service is not available");
  }

  m_iface = new QDBusInterface(DBUS_SERVICE_NAME, DBUS_OBJECT_PATH, "");
  if (!m_iface->isValid()) {
    QDBusError err = m_iface->lastError();
    delete m_iface;
    m_iface = nullptr;
    throw std::runtime_error(QString("%1: %2").arg(err.name()).arg(err.message()).toUtf8().data());
  }

  m_iface->setTimeout(DEFAULT_TIMEOUT);
}

bool DBusClient::isInterfaceAvailable() const
{
  return QDBusConnection::sessionBus().interface()->isServiceRegistered(DBUS_SERVICE_NAME);
}

bool DBusClient::loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath, const int loadOption)
{
  QDBusReply<IPCDBusDataPack> reply;

  if (hintPath == "")
    reply = m_iface->call("loadData", formatTag, loadOption);
  else
    reply = m_iface->call("loadDataHint", formatTag, hintPath, loadOption);

  if (!reply.isValid()) {
    QMessageBox::warning(nullptr, QObject::tr("No reply"),
                         QString(QObject::tr("CEval did not receive a reply from the data loader process. As a result no new data has been loaded. The exact error was:\n\n%1\n%2")).
                         arg(reply.error().name()).arg(reply.error().message()));

    return false;
  }

  const IPCDBusDataPack &pack = reply.value();

  for (const IPCDBusData &data : pack.data) {
    QVector<QPointF> nativeDatapoints = [](const QVector<IPCDBusDatapoint> &dpts) {
      QVector<QPointF> ndpts;

      ndpts.resize(dpts.size());
      for (const IPCDBusDatapoint &dp : dpts)
        ndpts.push_back(QPointF{dp.x, dp.y});

      return ndpts;
    }(data.datapoints);

    auto ndd = std::shared_ptr<EFGData>(new EFGData(nativeDatapoints,
                                                    data.xDescription, data.xUnit,
                                                    data.yDescription, data.yUnit));
    ndVec.push_back(NativeData{ndd, data.path, data.name});
  }

  return true;
}

bool DBusClient::supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats)
{
  QDBusReply<IPCDBusSupportedFileFormatVec> reply = m_iface->call("supportedFileFormats");

  if (!reply.isValid())
    return false;

  for (const auto &sff : reply.value()) {
    QMap<int, QString> loadOptions;

    for (int idx = 0; idx < sff.loadOptions.size(); idx++)
      loadOptions[idx] = sff.loadOptions.at(idx);

    supportedFormats.push_back(EFGSupportedFileFormat{sff.longDescription, sff.shortDescription, sff.tag, loadOptions});
  }

  return true;
}

DBusClient::~DBusClient()
{
  delete m_iface;
}

} //namespace efg
