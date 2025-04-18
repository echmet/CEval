#ifndef EFGLOADERINTERFACE_H
#define EFGLOADERINTERFACE_H

#include <QObject>
#include <QDataStream>
#include <QMap>
#include <QMetaType>
#include <QVector>
#include <memory>
#include <random>

#include "efgtypes.h"
#include "ipcclient.h"
#include "../datahash.h"

class QThread;

namespace efg {
  class EFGLoaderWatcher;
}

class TagPathPack {
public:
  QString tag;
  QString path;
};

inline
QDataStream & operator<<(QDataStream &out, const TagPathPack &p)
{
  out << p.tag << p.path;

  return out;
}

inline
QDataStream & operator>>(QDataStream &in, TagPathPack &p)
{
  in >> p.tag;
  in >> p.path;

  return in;
}

Q_DECLARE_METATYPE(TagPathPack)

using TagPathPackVec = QVector<TagPathPack>;
Q_DECLARE_METATYPE(TagPathPackVec)

typedef QVector<EFGSupportedFileFormat> EFGSupportedFileFormatVec;
Q_DECLARE_METATYPE(EFGSupportedFileFormatVec)

class EFGLoaderInterface : public QObject
{
  Q_OBJECT
public:
  ~EFGLoaderInterface();
  void loadUserSettings(const QVariant &settings);
  void retrieveSupportedFileFormats();
  QVariant saveUserSettings();

  static void initialize(const QString &ediiServicePath);
  static EFGLoaderInterface & instance();
  static void registerMetaTypes();

private:
  explicit EFGLoaderInterface(const QString &ediiServicePath, QObject *parent = nullptr);
  bool bringUpIPCInterface(const QString &ediiServicePath);
  void checkABICompatibility();
  QByteArray computeDataHash(const efg::IPCClient::NativeData &nd);

  std::unique_ptr<efg::GUIDisplayer> m_guiDisplayer;
  std::unique_ptr<efg::IPCClient> m_ipcClient;
  QMap<QString, QString> m_lastPathsMap;
  efg::EFGLoaderWatcher *m_watcher;
  QThread *m_myThread;
  std::mt19937_64 m_randEngine;

  static std::unique_ptr<EFGLoaderInterface> s_me;

  static const QString LAST_FILE_PATHS_SETTINGS_TAG;

signals:
  void onDataLoaded(EFGDataSharedPtr data, const DataHash &hash, const QString &path, const QString &name, const QString &id);
  void supportedFileFormatsRetrieved(EFGSupportedFileFormatVec supportedFormats);

public slots:
  void loadData(const QString &formatTag, const int loadOption);

private slots:
  void retrieveSupportedFileFormatsInternal();

};

#endif // EFGLOADERINTERFACE_H
