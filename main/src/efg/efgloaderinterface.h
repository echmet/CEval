#ifndef EFGLOADERINTERFACE_H
#define EFGLOADERINTERFACE_H

#include <QObject>
#include <QDataStream>
#include <QMap>
#include <QMetaType>
#include <QVector>
#include <memory>
#include "efgtypes.h"


#include "ipcclient.h"

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

class EFGLoaderInterface : public QObject
{
  Q_OBJECT
public:
  ~EFGLoaderInterface();
  static EFGLoaderInterface & instance();
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings();
  bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats);

private:
  explicit EFGLoaderInterface(QObject *parent = nullptr);

  efg::IPCClient *m_ipcClient;
  QMap<QString, QString> m_lastPathsMap;
  efg::EFGLoaderWatcher *m_watcher;

  static std::unique_ptr<EFGLoaderInterface> s_me;

  static const QString LAST_FILE_PATHS_SETTINGS_TAG;

signals:
  void onDataLoaded(std::shared_ptr<EFGData> &data, const QString &path, const QString &name);

public slots:
  void loadData(const QString &formatTag, const int loadOption);
};

#endif // EFGLOADERINTERFACE_H
