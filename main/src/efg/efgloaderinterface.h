#ifndef EFGLOADERINTERFACE_H
#define EFGLOADERINTERFACE_H

#include <QObject>
#include <memory>
#include "efgtypes.h"

#include "ipcclient.h"

namespace efg {
  class EFGLoaderWatcher;
}

class EFGLoaderInterface : public QObject
{
  Q_OBJECT
public:
  ~EFGLoaderInterface();
  static EFGLoaderInterface & instance();
  bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats);

private:
  explicit EFGLoaderInterface(QObject *parent = nullptr);

  efg::IPCClient *m_ipcClient;
  efg::EFGLoaderWatcher *m_watcher;

  static std::unique_ptr<EFGLoaderInterface> s_me;

signals:
  void onDataLoaded(std::shared_ptr<EFGData> &data, const QString &path, const QString &name);

public slots:
  void loadData(const QString &formatTag);
};

#endif // EFGLOADERINTERFACE_H
