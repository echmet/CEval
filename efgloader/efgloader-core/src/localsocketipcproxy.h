#ifndef UNIXSOCKETIPCPROXY_H
#define UNIXSOCKETIPCPROXY_H

#include "ipcproxy.h"
#include <QLocalServer>

class QLocalServer;
class QLocalSocket;

class LocalSocketIPCProxy : public IPCProxy
{
  Q_OBJECT

public:
  explicit LocalSocketIPCProxy(DataLoader *loader, QObject *parent = nullptr);
  virtual ~LocalSocketIPCProxy();

private:
  enum class RequestType {
    SUPPORTED_FORMATS,
    LOAD_DATA
  };

  void handleConnection(QLocalSocket *socket);
  bool readHeader(QLocalSocket *socket, RequestType &reqType);
  bool respondLoadData(QLocalSocket *socket);
  bool respondSupportedFormats(QLocalSocket *socket);

  QLocalServer *m_server;

private slots:
  void onNewConnection();

};

#endif // UNIXSOCKETIPCPROXY_H