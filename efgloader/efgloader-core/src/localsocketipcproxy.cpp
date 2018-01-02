#include "localsocketipcproxy.h"
#include "localsocketconnectionhandler.h"
#include "../common/ipcinterface.h"

#include <QThreadPool>

IPCServer::IPCServer(const DataLoader &loader, QObject *parent) :
  QLocalServer{parent},
  h_loader{loader}
{
  m_threadPool = new QThreadPool{this};
  m_threadPool->setMaxThreadCount(10);
}

void IPCServer::incomingConnection(quintptr sockDesc)
{
  if (sockDesc == 0)
    return;

  LocalSocketConnectionHandler *handler = new LocalSocketConnectionHandler{sockDesc, h_loader};
  handler->setAutoDelete(true);
  m_threadPool->start(handler);
}

LocalSocketIPCProxy::LocalSocketIPCProxy(DataLoader *loader, QObject *parent) :
  IPCProxy{loader, parent}
{
  m_server = new IPCServer{*m_loader, this};

  m_server->removeServer(IPCS_SOCKET_NAME);
  m_server->listen(IPCS_SOCKET_NAME);
}

LocalSocketIPCProxy::~LocalSocketIPCProxy()
{
}
