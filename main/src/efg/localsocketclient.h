#ifndef LOCALSOCKETCLIENT_H
#define LOCALSOCKETCLIENT_H

#include "ipcclient.h"
#include "../../../efgloader/core/common/ipcinterface.h"

class QLocalSocket;

namespace efg {

class LocalSocketClient : public IPCClient
{
public:
  explicit LocalSocketClient();
  virtual ~LocalSocketClient();

  virtual bool loadData(NativeDataVec &ndVec, const QString &formatTag) override;
  virtual bool supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats) override;

private:
  template <typename Packet> bool checkSignature(const Packet *p, const IPCSockResponseType rtype);
  template <typename Packet> bool checkResponse(const Packet *p, const IPCSockResponseType rtype, QString &error);
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
