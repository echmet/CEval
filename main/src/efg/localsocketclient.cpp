#include "localsocketclient.h"
#include <QLocalSocket>
#include <QThread>

#define FAIL(socket) \
  do { \
    socket->close(); \
    return false; \
  } while (false)

#define FINALIZE(socket) \
  do { \
    socket->close(); \
    return true; \
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

LocalSocketClient::LocalSocketClient() : IPCClient(),
  m_socket(nullptr)
{
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

void LocalSocketClient::connectToInterface()
{
  if (!connectSocket())
    throw std::runtime_error(QString("IPC socket timeout while waiting for connection (%1)").arg(m_socket->errorString()).toUtf8().data());
}

bool LocalSocketClient::connectSocket()
{
  const int max = 24;
  int ctr = 0;

  if (m_socket == nullptr)
    m_socket = new QLocalSocket();

  while (m_socket->state() != QLocalSocket::ConnectedState && ctr < max) {
     m_socket->connectToServer(QString(IPCS_SOCKET_NAME));
     if (m_socket->waitForConnected(1000))
       return true;
#ifdef Q_OS_WIN
     QThread::msleep(1000); /* waitForConnected() does not block on Windows */
#else
     QThread::msleep(100);
#endif // Q_OS_WIN
     ctr++;
  }
  if (ctr >= max)
    return false;

  return true;
}

bool LocalSocketClient::isInterfaceAvailable()
{
  bool ifaceAvailable;

  m_socket = new QLocalSocket();
  m_socket->connectToServer(QString(IPCS_SOCKET_NAME));
#ifdef Q_OS_WIN
     QThread::msleep(100); /* Insert a mandatory delay here since waitForConnected() does not block on Windows */
#endif // Q_OS_WIN
  ifaceAvailable = m_socket->waitForConnected(100);

  /* We need to recreate the socket from the EFGLoader event queue in order to
   * have it assigned to the correct thread */
  delete m_socket;
  m_socket = nullptr;

  return ifaceAvailable;
}

bool LocalSocketClient::loadData(NativeDataVec &ndVec, const QString &formatTag, const QString &hintPath, const int loadOption)
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
  reqLdrHdr.loadOption = loadOption;
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
    QByteArray dataIdBA;

    if (!readDynBlock(nameBA, ldrDesc->nameLength))
      FAIL(m_socket);
    if (!readDynBlock(dataIdBA, ldrDesc->dataIdLength))
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
    nativeDatapoints.reserve(ldrDesc->datapointsLength);
    for (uint32_t idx = 0; idx < ldrDesc->datapointsLength; idx++) {
      const IPCSockDatapoint *dp = reinterpret_cast<const IPCSockDatapoint *>(datapoints.data() + (idx * sizeof(IPCSockDatapoint)));

      nativeDatapoints.push_back(QPointF(dp->x, dp->y));
    }

    auto efgData = std::shared_ptr<EFGData>(new EFGData{nativeDatapoints,
                                                        QString::fromUtf8(xDescBA), QString::fromUtf8(xUnitBA),
                                                        QString::fromUtf8(yDescBA), QString::fromUtf8(yUnitBA),
                                                        QString::fromUtf8(dataIdBA)});
    ndVec.push_back(NativeData(efgData, QString::fromUtf8(pathBA), QString::fromUtf8(nameBA)));
  }

  FINALIZE(m_socket);
}

bool LocalSocketClient::readBlock(QByteArray &buffer, const int size)
{
  if (!waitForData(size))
    return false;

  buffer.resize(size);
  qint64 read = 0;
  while (read < size) {
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
  if (m_socket == nullptr)
    return connectSocket();

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
    FINALIZE(m_socket);

  int32_t item;
  for (item = 0; item < respHdr->items; item++) {
    QByteArray itemBA;
    const IPCSockSupportedFormatResponseDescriptor *sfrDesc = readPacket<IPCSockSupportedFormatResponseDescriptor>(itemBA);
    if (sfrDesc == nullptr)
      FAIL(m_socket);
    if ((!checkSignature(sfrDesc, IPCSockResponseType::RESPONSE_SUPPORTED_FORMAT_DESCRIPTOR)) || sfrDesc->status != IPCS_SUCCESS)
      FAIL(m_socket);

    QByteArray longDescriptionBA;
    QByteArray shortDescriptionBA;
    QByteArray tagBA;

    if (!readBlock(longDescriptionBA, sfrDesc->longDescriptionLength))
      FAIL(m_socket);
    if (!readBlock(shortDescriptionBA, sfrDesc->shortDescriptionLength))
      FAIL(m_socket);
    if (!readBlock(tagBA, sfrDesc->tagLength))
      FAIL(m_socket);

    QMap<int, QString> loadOptions;
    if (sfrDesc->loadOptionsLength > 1) {
      for (uint32_t ctr = 0; ctr < sfrDesc->loadOptionsLength; ctr++) {
        QByteArray loDescBA;
        const IPCSockLoadOptionDescriptor *loDesc = readPacket<IPCSockLoadOptionDescriptor>(loDescBA);
        if (loDesc == nullptr)
          FAIL(m_socket);
        if (!checkSignature(loDesc, IPCSockResponseType::RESPONSE_LOAD_OPTION_DESCRIPTOR))
          FAIL(m_socket);

        QByteArray loadOptionBA;
        if (!readBlock(loadOptionBA, loDesc->optionLength))
          FAIL(m_socket);

        loadOptions[ctr] = QString::fromUtf8(loadOptionBA);
      }
    }

    supportedFormats.push_back(EFGSupportedFileFormat(QString::fromUtf8(longDescriptionBA), QString::fromUtf8(shortDescriptionBA), QString::fromUtf8(tagBA), loadOptions));
  }

  if (item != respHdr->items)
    m_socket->close();

  FINALIZE(m_socket);
}

bool LocalSocketClient::waitForData(const int size)
{
  qint64 avail = m_socket->bytesAvailable();
  while (avail < size) {
    const auto state = m_socket->state();
    if (!(state == QLocalSocket::ConnectedState || state == QLocalSocket::ClosingState))
      return false;

    m_socket->waitForReadyRead(100);
    avail = m_socket->bytesAvailable();
  }

  return true;
}

} // namespace efg
