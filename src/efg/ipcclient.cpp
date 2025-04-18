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

IPCClient::NativeData::NativeData(const NativeData &other) :
  data(other.data),
  path(other.path),
  name(other.name)
{
}

IPCClient::NativeData::NativeData(NativeData &&other) noexcept :
  data(std::move(other.data)),
  path(std::move(other.path)),
  name(std::move(other.name))
{
}

IPCClient::NativeData & IPCClient::NativeData::operator=(NativeData &&other) noexcept
{
  this->data = std::move(other.data);
  const_cast<QString&>(this->path) = std::move(other.path);
  const_cast<QString&>(this->name) = std::move(other.name);

  return *this;
}

IPCClient::IPCClient()
{
}

IPCClient::~IPCClient()
{
}

} // namespace efg
