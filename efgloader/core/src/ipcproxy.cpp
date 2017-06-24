#include "ipcproxy.h"
#include "dataloader.h"

IPCProxy::IPCProxy(DataLoader *loader, QObject *parent) :
  QObject(parent),
  m_loader(loader)
{
}

IPCProxy::~IPCProxy()
{
}
