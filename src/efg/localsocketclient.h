#ifndef LOCALSOCKETCLIENT_H
#define LOCALSOCKETCLIENT_H

#include "ipcclient.h"
#include <edii_ipc_network.h>

class QLocalSocket;

namespace efg {

class LocalSocketClient : public IPCClient
{
public:
  explicit LocalSocketClient();
  virtual ~LocalSocketClient() override;

  virtual void connectToInterface() override;
  virtual bool isInterfaceAvailable() override;
  virtual bool loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath, const int loadOption) override;
  virtual bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats) override;

private:
  template <typename Packet> bool checkSignature(const Packet *p, const EDII_IPCSockResponseType rtype);
  template <typename Packet> bool checkResponse(const Packet *p, const EDII_IPCSockResponseType rtype, QString &error);
  bool connectSocket();
  bool reconnectIfNeeded();
  bool readBlock(QByteArray &buffer, const int size);
  template <typename Packet> const Packet * readPacket(QByteArray &buffer);
  bool sendBlock(const QByteArray &payload);
  template <typename Packet> bool sendPacket(const Packet &packet);
  bool waitForData(const int size);

  QLocalSocket *m_socket;
};

} // namespace efg

#endif // LOCALSOCKETCLIENT_H
