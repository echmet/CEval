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

  friend QDataStream & operator<<(QDataStream &out, const TagPathPack &p)
  {
    out << p.tag << p.path;

    return out;
  }

  friend QDataStream & operator>>(QDataStream &in, TagPathPack &p)
  {
    in >> p.tag;
    in >> p.path;

    return in;
  }
};
Q_DECLARE_METATYPE(TagPathPack)

class TagPathPackVec : public QVector<TagPathPack> {
  friend QDataStream & operator<<(QDataStream &out, const TagPathPackVec &vec)
  {
    out << vec.size();
    for (const auto & item : vec)
      out << item;

    return out;
  }

  friend QDataStream & operator>>(QDataStream &in, TagPathPackVec &vec)
  {
    qint32 dataSize;
    qint32 ctr = 0;

    in >> dataSize;

    while (!in.atEnd() && ctr < dataSize) {
      TagPathPack p;

      in >> p.tag;
      in >> p.path;

      vec.push_back(p);
      ctr++;
    }

    return in;
  }
};
Q_DECLARE_METATYPE(TagPathPackVec)

typedef QVector<EFGSupportedFileFormat> EFGSupportedFileFormatVec;

class EFGLoaderInterface : public QObject
{
  Q_OBJECT
public:
  ~EFGLoaderInterface();
  void loadUserSettings(const QVariant &settings);
  void retrieveSupportedFileFormats();
  QVariant saveUserSettings();

  static void initialize();
  static EFGLoaderInterface & instance();

private:
  explicit EFGLoaderInterface(QObject *parent = nullptr);
  bool bringUpIPCInterface();
  QByteArray computeDataHash(const efg::IPCClient::NativeData &nd);

  std::unique_ptr<efg::IPCClient> m_ipcClient;
  QMap<QString, QString> m_lastPathsMap;
  efg::EFGLoaderWatcher *m_watcher;
  QThread *m_myThread;
  std::mt19937_64 m_randEngine;

  static std::unique_ptr<EFGLoaderInterface> s_me;

  static const QString LAST_FILE_PATHS_SETTINGS_TAG;

signals:
  void onDataLoaded(EFGDataSharedPtr data, const DataHash &hash, const QString &path, const QString &name);
  void supportedFileFormatsRetrieved(EFGSupportedFileFormatVec supportedFormats);

public slots:
  void loadData(const QString &formatTag, const int loadOption);

private slots:
  void retrieveSupportedFileFormatsInternal();

};

#endif // EFGLOADERINTERFACE_H
