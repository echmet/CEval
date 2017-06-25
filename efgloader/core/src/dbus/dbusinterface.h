#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#ifdef ENABLE_IPC_INTERFACE_DBUS

#include "common/ipcinterface.h"
#include <QObject>

class DBusInterface : public QObject {
  Q_OBJECT

  Q_CLASSINFO("CEval electrophoregram loader D-Bus interface", "dataloader.loader")

public:
  enum class LoadMode {
    INTERACTIVE,
    HINT,
    FILE
  };
  Q_ENUM(LoadMode)

  explicit DBusInterface(QObject *parent = nullptr);
  virtual ~DBusInterface();

public slots:
  IPCDBusDataPack loadData(const QString &formatTag, const int loadOption);
  IPCDBusDataPack loadDataHint(const QString &formatTag, const QString &hint, const int loadOption);
  IPCDBusDataPack loadDataFile(const QString &formatTag, const QString &filePath, const int loadOption);
  IPCDBusSupportedFileFormatVec supportedFileFormats();

signals:
  void loadDataForwarder(IPCDBusDataPack &pack, const QString &formatTag, const LoadMode mode, const QString &modeParam, const int loadOption);
  void supportedFileFormatsForwarder(IPCDBusSupportedFileFormatVec &supportedFileFormats);
};

#endif // USE_IPC_INTERFACE_DBUS

#endif // DBUSINTERFACE_H
