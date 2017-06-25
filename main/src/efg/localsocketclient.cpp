#include "localsocketclient.h"
#include <QLocalSocket>
#include <QThread>

#define FAIL(socket) \
  do { \
    socket->close(); \
    return false; \
  } while (false)

#define WAIT_FOR_WRITTEN(socket) \
  do { \
    if (!socket->waitForBytesWritten()) \
    return false; \
  } while (false)

namespace efg {

template <typename Packet>
void initRequest(Packet &p, IPCSockRequestType rtype)
{
  p.magic = IPCS_PACKET_MAGIC;
  p.requestType = rtype;
}

template <typename Packet>
QByteArray wrap(const Packet &p)
{
  return QByteArray(reinterpret_cast<const char *>(&p), sizeof(p));
}

LocalSocketClient::LocalSocketClient() : IPCClient()
{
  m_socket = new QLocalSocket();

  if (!connectSocket())
     throw std::runtime_error(QString("IPC socket timeout while waiting for connection (%1)").arg(m_socket->errorString()).toUtf8().data());
}

LocalSocketClient::~LocalSocketClient()
{
  delete m_socket;
}

template <typename Packet>
bool LocalSocketClient::checkSignature(const Packet *p, const IPCSockResponseType rtype)
{
  return (p->magic == IPCS_PACKET_MAGIC) && (p->responseType == rtype);
}

template <typename Packet>
bool LocalSocketClient::checkResponse(const Packet *p, const IPCSockResponseType rtype, QString &error)
{
  if (!checkSignature(p, rtype)) {
    error = QObject::tr("Invalid packet signature");
    return false;
  }

  if (p->status == IPCS_FAILURE) {
    if (p->errorLength > 0) {
      QByteArray errorBA;

      if (!readBlock(errorBA, p->errorLength)) {
        error = QObject::tr("Cannot retrieve error message");
        return false;
      }

      error = QString::fromUtf8(errorBA);
      return false;
    }
  } else if (p->status == IPCS_SUCCESS)
    return true;

  error = QObject::tr("Invalid response status code");
  return false;
}

bool LocalSocketClient::connectSocket()
{
 const int max = 10;
  int ctr = 0;
  while (m_socket->state() != QLocalSocket::ConnectedState && ctr < max) {
     m_socket->connectToServer(QString(IPCS_SOCKET_NAME));
     m_socket->waitForConnected(1000);
     QThread::msleep(100);
     ctr++;
  }
  if (ctr >= max)
    return false;

  return true;
}

bool LocalSocketClient::loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath)
{
  if (!reconnectIfNeeded())
    return false;

  QString error;

  IPCSockRequestHeader reqHdr;
  initRequest(reqHdr, IPCSockRequestType::REQUEST_LOAD_DATA);
  if (!sendPacket(reqHdr))
    FAIL(m_socket);

  QByteArray formatTagBA = formatTag.toUtf8();
  IPCSockLoadDataRequestDescriptor reqLdrHdr;
  initRequest(reqLdrHdr, IPCSockRequestType::REQUEST_LOAD_DATA_DESCRIPTOR);
  reqLdrHdr.tagLength = formatTagBA.size();

  QByteArray hintPathBA = hintPath.toUtf8();
  if (hintPath.length() > 0) {
    reqLdrHdr.mode = IPCSocketLoadDataMode::IPCS_LOAD_HINT;
    reqLdrHdr.filePathLength = hintPathBA.size();
  } else {
    reqLdrHdr.mode = IPCSocketLoadDataMode::IPCS_LOAD_INTERACTIVE;
    reqLdrHdr.filePathLength = 0;
  }

  if (!sendPacket(reqLdrHdr))
    FAIL(m_socket);
  if (!sendBlock(formatTagBA))
    FAIL(m_socket);
  if (reqLdrHdr.filePathLength > 0) {
    if (!sendBlock(hintPathBA))
      FAIL(m_socket);
  }


  QByteArray respLdrHdrBA;
  const IPCSockResponseHeader *respLdrHdr = readPacket<IPCSockResponseHeader>(respLdrHdrBA);
  if (respLdrHdr == nullptr)
    FAIL(m_socket);
  if (!checkResponse(respLdrHdr, IPCSockResponseType::RESPONSE_LOAD_DATA_HEADER, error)) {
    qWarning() << error;
    FAIL(m_socket);
  }

  int32_t item;
  for (item = 0; item < respLdrHdr->items; item++) {
    auto readDynBlock = [this](QByteArray &buf, const int size) {
        if (size < 1)
          return true;

        return readBlock(buf, size);
    };
    auto readDatapoints = [this](QByteArray &buf, const int size) {
      if (size < 1)
        return true;

      return readBlock(buf, size * sizeof(IPCSockDatapoint));
    };

    QByteArray ldrDescBA;
    const IPCSockLoadDataResponseDescriptor *ldrDesc = readPacket<IPCSockLoadDataResponseDescriptor>(ldrDescBA);
    if (ldrDesc == nullptr)
      FAIL(m_socket);
    if (!checkSignature(ldrDesc, IPCSockResponseType::RESPONSE_LOAD_DATA_DESCRIPTOR))
      FAIL(m_socket);
    if (ldrDesc->status != IPCS_SUCCESS)
      continue;

    QByteArray nameBA;
    QByteArray pathBA;
    QByteArray xDescBA;
    QByteArray yDescBA;
    QByteArray xUnitBA;
    QByteArray yUnitBA;

    if (!readDynBlock(nameBA, ldrDesc->nameLength))
      FAIL(m_socket);
    if (!readDynBlock(pathBA, ldrDesc->pathLength))
      FAIL(m_socket);
    if (!readDynBlock(xDescBA, ldrDesc->xDescriptionLength))
      FAIL(m_socket);
    if (!readDynBlock(yDescBA, ldrDesc->yDescriptionLength))
      FAIL(m_socket);
    if (!readDynBlock(xUnitBA, ldrDesc->xUnitLength))
      FAIL(m_socket);
    if (!readDynBlock(yUnitBA, ldrDesc->yUnitLength))
      FAIL(m_socket);

    QByteArray datapoints;
    if (!readDatapoints(datapoints, ldrDesc->datapointsLength))
      FAIL(m_socket);

    QVector<QPointF> nativeDatapoints;
    for (uint32_t idx = 0; idx < ldrDesc->datapointsLength; idx++) {
      const IPCSockDatapoint *dp = reinterpret_cast<const IPCSockDatapoint *>(datapoints.data() + (idx * sizeof(IPCSockDatapoint)));

      nativeDatapoints.push_back(QPointF(dp->x, dp->y));
    }

    auto efgData = std::shared_ptr<EFGData>(new EFGData(nativeDatapoints,
                                                        QString::fromUtf8(xDescBA), QString::fromUtf8(yUnitBA),
                                                        QString::fromUtf8(yDescBA), QString::fromUtf8(yUnitBA)));
    ndVec.push_back(NativeData(efgData, QString::fromUtf8(pathBA), QString::fromUtf8(nameBA)));
  }

  return true;
}

bool LocalSocketClient::readBlock(QByteArray &buffer, const int size)
{
  waitForData(size);

  buffer.resize(size);
  qint64 read = 0;
  while (read < size) {
    if (m_socket->state() != QLocalSocket::ConnectedState)
      return false;

    qint64 r = m_socket->read(buffer.data() + read, size);
    if (r <= 0)
      return false;
    read += r;
  }

  return true;
}

template <typename Packet>
const Packet * LocalSocketClient::readPacket(QByteArray &buffer)
{
  const int size = sizeof(Packet);

  if (!readBlock(buffer, size))
    return nullptr;

  return reinterpret_cast<const Packet *>(buffer.data());
}

bool LocalSocketClient::reconnectIfNeeded()
{
  if (m_socket->state() != QLocalSocket::ConnectedState) {
    if (!connectSocket())
      return false;
  }

  return true;
}

bool LocalSocketClient::sendBlock(const QByteArray &payload)
{
  if (m_socket->write(payload) < payload.size())
    return false;

  WAIT_FOR_WRITTEN(m_socket);
  return true;
}

template <typename Packet>
bool LocalSocketClient::sendPacket(const Packet &packet)
{
  const QByteArray bytes = wrap(packet);

  return sendBlock(bytes);
}

bool LocalSocketClient::supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats)
{
  if (!reconnectIfNeeded())
    return false;

  QString error;

  IPCSockRequestHeader reqHdr;
  initRequest(reqHdr, IPCSockRequestType::REQUEST_SUPPORTED_FORMATS);
  if (!sendPacket(reqHdr))
    FAIL(m_socket);

  QByteArray respHrdBA;
  const IPCSockResponseHeader *respHdr = readPacket<IPCSockResponseHeader>(respHrdBA);
  if (respHdr == nullptr)
    FAIL(m_socket);
  if (!checkResponse(respHdr, IPCSockResponseType::RESPONSE_SUPPORTED_FORMAT_HEADER, error)) {
    qWarning() << error;
    FAIL(m_socket);
  }

  if (respHdr->items < 1)
    return true;

  int32_t item;
  for (item = 0; item < respHdr->items; item++) {
    QByteArray itemBA;
    const IPCSockSupportedFormatResponseDescriptor *sfrDesc = readPacket<IPCSockSupportedFormatResponseDescriptor>(itemBA);
    if (sfrDesc == nullptr)
      FAIL(m_socket);
    if ((!checkSignature(sfrDesc, IPCSockResponseType::RESPONSE_SUPPORTED_FORMAT_DESCRIPTOR)) || sfrDesc->status != IPCS_SUCCESS)
      FAIL(m_socket);

    QByteArray descriptionBA;
    QByteArray tagBA;

    if (!readBlock(descriptionBA, sfrDesc->descriptionLength))
      FAIL(m_socket);
    if (!readBlock(tagBA, sfrDesc->tagLength))
      FAIL(m_socket);

    supportedFormats.push_back(EFGSupportedFileFormat(QString::fromUtf8(tagBA), QString::fromUtf8(descriptionBA), QString::fromUtf8(tagBA)));
  }

  if (item != respHdr->items)
    m_socket->close();

  return true;
}

bool LocalSocketClient::waitForData(const int size)
{
  while (m_socket->bytesAvailable() < size) {
    if (m_socket->state() != QLocalSocket::ConnectedState)
      return false;

    m_socket->waitForReadyRead(100);
  }

  return true;
}

} // namespace efg
