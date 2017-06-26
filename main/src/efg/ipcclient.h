#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <memory>
#include "efgtypes.h"

namespace efg {

class IPCClient
{
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
  virtual bool isInterfaceAvailable() const = 0;
  virtual bool loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath, const int loadOption) = 0;
  virtual bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats) = 0;

};

}

#endif // IPCCLIENT_H
