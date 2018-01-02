#ifndef UNIXSOCKETIPCPROXY_H
#define UNIXSOCKETIPCPROXY_H

#include "ipcproxy.h"
#include <QLocalServer>

class QThreadPool;

class IPCServer : public QLocalServer
{
  Q_OBJECT

public:
  explicit IPCServer(const DataLoader &loader, QObject *parent);
  virtual void incomingConnection(quintptr sockDesc) override;

private:
  const DataLoader &h_loader;
  QThreadPool *m_threadPool;
};

class LocalSocketIPCProxy : public IPCProxy
{
  Q_OBJECT

public:
  explicit LocalSocketIPCProxy(DataLoader *loader, QObject *parent = nullptr);
  virtual ~LocalSocketIPCProxy() override;

private:
  IPCServer *m_server;
};

#endif // UNIXSOCKETIPCPROXY_H
