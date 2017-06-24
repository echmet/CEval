#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent) :
  QObject(parent)
{
}

DBusInterface::~DBusInterface()
{
}

IPCDBusDataPack DBusInterface::loadData(const QString &formatTag)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::INTERACTIVE, "");

  return pack;
}

IPCDBusDataPack DBusInterface::loadDataHint(const QString &formatTag, const QString &hint)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::HINT, hint);

  return pack;
}

IPCDBusDataPack DBusInterface::loadDataFile(const QString &formatTag, const QString &filePath)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::FILE, filePath);

  return pack;
}

IPCDBusSupportedFileFormatVec DBusInterface::supportedFileFormats()
{
  IPCDBusSupportedFileFormatVec vec;
  emit supportedFileFormatsForwarder(vec);

  return vec;
}
