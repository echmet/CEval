#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent) :
  QObject(parent)
{
}

DBusInterface::~DBusInterface()
{
}

IPCDBusDataPack DBusInterface::loadData(const QString &formatTag, const int loadOption)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::INTERACTIVE, "", loadOption);

  return pack;
}

IPCDBusDataPack DBusInterface::loadDataHint(const QString &formatTag, const QString &hint, const int loadOption)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::HINT, hint, loadOption);

  return pack;
}

IPCDBusDataPack DBusInterface::loadDataFile(const QString &formatTag, const QString &filePath, const int loadOption)
{
  IPCDBusDataPack pack;

  emit loadDataForwarder(pack, formatTag, LoadMode::FILE, filePath, loadOption);

  return pack;
}

IPCDBusSupportedFileFormatVec DBusInterface::supportedFileFormats()
{
  IPCDBusSupportedFileFormatVec vec;
  emit supportedFileFormatsForwarder(vec);

  return vec;
}
