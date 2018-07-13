#include "ipcclient.h"

namespace efg {

IPCClient::NativeData::NativeData() :
  data(nullptr),
  path(""),
  name("")
{
}

IPCClient::NativeData::NativeData(std::shared_ptr<EFGData> &data, const QString &path, const QString &name) :
  data(data),
  path(path),
  name(name)
{
}

IPCClient::IPCClient()
{
}

IPCClient::~IPCClient()
{
}

} // namespace efg
