#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <QObject>
#include <memory>
#include "efgtypes.h"

namespace efg {

class IPCClient : public QObject {
  Q_OBJECT
public:
  class NativeData {
  public:
    explicit NativeData();
    explicit NativeData(std::shared_ptr<EFGData> &data, const QString &path, const QString &name);

    std::shared_ptr<EFGData> data;
    const QString path;
    const QString name;
  };
  typedef QVector<NativeData> NativeDataVec;

  explicit IPCClient();
  virtual ~IPCClient();
  virtual void connectToInterface() = 0;
  virtual bool isInterfaceAvailable() = 0;
  virtual bool loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath, const int loadOption) = 0;
  virtual bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats) = 0;

signals:
  void displayWarning(const QString &title, const QString &message);
};

}

#endif // IPCCLIENT_H
