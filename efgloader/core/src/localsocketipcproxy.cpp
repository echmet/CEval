#include "localsocketipcproxy.h"
#include "../common/ipcinterface.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>

#ifdef Q_OS_WIN
  #define HANDLING_TIMEOUT 2500
#else
  #define HANDLING_TIMEOUT 250
#endif // Q_OS_WIN

#ifdef Q_OS_WIN
  #define FINALIZE(socket) \
    if (socket->bytesToWrite()) \
      return socket->waitForBytesWritten(); \
    return true;
#else
   #define FINALIZE(socket) \
     return socket->waitForBytesWritten()
#endif // Q_OS_WIN


#define INIT_RESPONSE(packet, rtype, s) \
  packet.magic = IPCS_PACKET_MAGIC; \
  packet.responseType = rtype; \
  packet.status = s

#define WAIT_FOR_DATA(socket) \
  if (!socket->bytesAvailable()) { \
    if (!socket->waitForReadyRead(1000)) { \
      qWarning() << "Timed out while waiting for data block"; \
      return false; \
    } \
  }

#define WRITE_CHECKED(socket, payload) \
  if (!writeSegmented(socket, payload.data(), payload.size())) { \
    qWarning() << "Failed to send payload:" << socket->errorString(); \
    return false; \
  }

#define WRITE_CHECKED_RAW(socket, payload) \
  if (!writeSegmented(socket, (const char*)&payload, static_cast<qint64>(sizeof(payload)))) { \
    qWarning() << "Failed to send raw payload:" << socket->errorString(); \
    return false; \
  }

#define WRITE_RAW(socket, payload) \
  writeSegmented(socket, (const char*)&payload, static_cast<qint64>(sizeof(payload)))

template <typename P>
bool checkSig(const P &packet)
{
  return packet->magic == IPCS_PACKET_MAGIC;
}

template <typename P, typename R>
bool checkSig(const P &packet, const R requestType)
{
  return checkSig(packet) && packet->requestType == requestType;
}

bool writeSegmented(QLocalSocket *socket, const char *payload, const qint64 bytesToWrite)
{
#ifdef Q_OS_WIN
  const qint64 MAX_SEGMENT_SIZE = 4095;
  qint64 written = 0;

  while (written < bytesToWrite) {
    const qint64 writeMax = (bytesToWrite - written) > MAX_SEGMENT_SIZE ? MAX_SEGMENT_SIZE : (bytesToWrite - written);
    const qint64 w = socket->write(payload + written, writeMax);
    if (w < 1)
      return false;
    written += w;

    if (!socket->waitForBytesWritten())
      return false;
  }

  return true;
#else
  return socket->write(payload, bytesToWrite);
#endif // Q_OS_WIN
}

bool isSocketAlive(QLocalSocket *socket)
{
#ifdef Q_OS_WIN
  Q_UNUSED(socket);
  return false; /* There is something horribly broken on Windows, we have to renegotiate the socket every time. */
#else
  return socket->error() == QLocalSocket::SocketTimeoutError;
#endif // Q_OS_WIN
}

bool reportError(QLocalSocket *socket, const IPCSockResponseType rtype, const QString &message)
{
  IPCSockResponseHeader resp;
  QByteArray messageRaw(message.toUtf8());

  INIT_RESPONSE(resp, rtype, IPCS_FAILURE);
  resp.errorLength = messageRaw.size();

  WRITE_CHECKED_RAW(socket, resp);
  WRITE_CHECKED(socket, messageRaw);
  FINALIZE(socket);
}

bool readBlock(QLocalSocket *socket, QByteArray &buffer, qint64 size)
{
  buffer.resize(size);
  qint64 read = 0;
  while (read < size) {
    if (socket->state() != QLocalSocket::ConnectedState)
      return false;

    qint64 r = socket->read(buffer.data() + read, size);
    if (r < 0) {
      qWarning() << "Unable to read block:"<< socket->errorString();
      return false;
    }
    read += r;
  }

  return true;
}

LocalSocketIPCProxy::LocalSocketIPCProxy(DataLoader *loader, QObject *parent) :
  IPCProxy(loader, parent)
{
  m_server = new QLocalServer{this};

  connect(m_server, &QLocalServer::newConnection, this ,&LocalSocketIPCProxy::onNewConnection);

  m_server->removeServer(IPCS_SOCKET_NAME);
  m_server->listen(IPCS_SOCKET_NAME);
}

LocalSocketIPCProxy::~LocalSocketIPCProxy()
{
}

void LocalSocketIPCProxy::onNewConnection()
{
  QLocalSocket *socket = m_server->nextPendingConnection();

  handleConnection(socket);
  socket->close();
}

void LocalSocketIPCProxy::handleConnection(QLocalSocket *socket)
{
  while (true) {
    if (!socket->bytesAvailable()) {
      if (!socket->waitForReadyRead(HANDLING_TIMEOUT)) {
        if (isSocketAlive(socket))
          continue;
        else
          return;
      }
    }

    RequestType reqType;
    if (!readHeader(socket, reqType))
      return;

    switch (reqType) {
    case RequestType::SUPPORTED_FORMATS:
      if (!respondSupportedFormats(socket))
        return;
      break;
    case RequestType::LOAD_DATA:
      if (!respondLoadData(socket))
       return;
      break;
    }
  };
}

bool LocalSocketIPCProxy::readHeader(QLocalSocket *socket, RequestType &reqType)
{
  static const qint64 HEADER_SIZE = sizeof(IPCSockRequestHeader);
  QByteArray headerRaw;

  if (!readBlock(socket, headerRaw, HEADER_SIZE)) {
    qWarning() << "Cannot read request header";
    return false;
  }

  const IPCSockRequestHeader *header = (const IPCSockRequestHeader *)(headerRaw.data());

  if (!checkSig(header))
    return false;

  switch (header->requestType) {
  case IPCSockRequestType::REQUEST_SUPPORTED_FORMATS:
    reqType = RequestType::SUPPORTED_FORMATS;
    break;
  case IPCSockRequestType::REQUEST_LOAD_DATA:
    reqType = RequestType::LOAD_DATA;
    break;
  default:
    return false;
  }

  return true;
}

bool LocalSocketIPCProxy::respondLoadData(QLocalSocket *socket)
{
  static const qint64 REQ_DESC_SIZE = sizeof(IPCSockLoadDataRequestDescriptor);
  QString formatTag;
  QString path;

  /* Read request descriptor */
  WAIT_FOR_DATA(socket);
  IPCSockLoadDataRequestDescriptor *reqDesc;
  QByteArray reqDescRaw;
  if (!readBlock(socket, reqDescRaw, REQ_DESC_SIZE)) {
    qWarning() << "Cannot read load data descriptor";
    return false;
  }
  reqDesc = (IPCSockLoadDataRequestDescriptor *)reqDescRaw.data();
  if (!checkSig(reqDesc, REQUEST_LOAD_DATA_DESCRIPTOR)) {
    qWarning() << "Invalid load data descriptor signature";
    return false;
  }
  if (reqDesc->tagLength < 1) {
    qWarning() << "Invalid length of formatTag";
    reportError(socket, RESPONSE_LOAD_DATA_HEADER, "Invalid length of formatTag");
    return false;
  }

  /* Read tag */
  WAIT_FOR_DATA(socket)
  QByteArray tagRaw;
  if (!readBlock(socket, tagRaw, reqDesc->tagLength)) {
    qWarning() << "Cannot read format tag";
    return false;
  }
  formatTag = QString::fromUtf8(tagRaw);

  switch (reqDesc->mode) {
  case IPCSocketLoadDataMode::IPCS_LOAD_FILE:
  case IPCSocketLoadDataMode::IPCS_LOAD_HINT:
  {
    if (reqDesc->filePathLength > 0) {
      WAIT_FOR_DATA(socket);
      QByteArray pathRaw;
      if (!readBlock(socket, pathRaw, reqDesc->filePathLength)) {
        qWarning() << "Cannot read file path";
        return false;
      }

      path = QString::fromUtf8(pathRaw);
    } else {
      reportError(socket, RESPONSE_LOAD_DATA_HEADER, "Invalid file path length");
      return false;
    }
  }
    break;
  case IPCSocketLoadDataMode::IPCS_LOAD_INTERACTIVE:
    break;
  default:
    reportError(socket, RESPONSE_LOAD_DATA_HEADER, "Invalid load mode");
    return false;
  }

  DataLoader::LoadedPack result;
  switch (reqDesc->mode) {
  case IPCSocketLoadDataMode::IPCS_LOAD_INTERACTIVE:
    result = m_loader->loadData(formatTag, reqDesc->loadOption);
    break;
  case IPCSocketLoadDataMode::IPCS_LOAD_HINT:
    result = m_loader->loadDataHint(formatTag, path, reqDesc->loadOption);
    break;
  case IPCSocketLoadDataMode::IPCS_LOAD_FILE:
    result = m_loader->loadDataPath(formatTag, path, reqDesc->loadOption);
    break;
  }

  /* We have the data (or a failure), report it back */
  IPCSockResponseHeader respHeader;
  if (!std::get<1>(result)) {
    INIT_RESPONSE(respHeader, IPCSockResponseType::RESPONSE_LOAD_DATA_HEADER, IPCS_FAILURE);
    const QByteArray error = std::get<2>(result).toUtf8();

    respHeader.errorLength = error.size();

    WRITE_RAW(socket, respHeader);

    socket->write(error);
    FINALIZE(socket);
  }

  const std::vector<Data> &data = std::get<0>(result);
  INIT_RESPONSE(respHeader, IPCSockResponseType::RESPONSE_LOAD_DATA_HEADER, IPCS_SUCCESS);
  respHeader.items = data.size();
  WRITE_CHECKED_RAW(socket, respHeader);

  for (const auto &item : data) {
    IPCSockLoadDataResponseDescriptor respDesc;
    INIT_RESPONSE(respDesc, IPCSockResponseType::RESPONSE_LOAD_DATA_DESCRIPTOR, IPCS_SUCCESS);

    QByteArray nameBytes = item.name.toUtf8();
    QByteArray pathBytes = item.path.toUtf8();;
    QByteArray xDescBytes = item.xDescription.toUtf8();
    QByteArray yDescBytes = item.yDescription.toUtf8();
    QByteArray xUnitBytes = item.xUnit.toUtf8();
    QByteArray yUnitBytes = item.yUnit.toUtf8();

    respDesc.nameLength = nameBytes.size();
    respDesc.pathLength = pathBytes.size();
    respDesc.xDescriptionLength = xDescBytes.size();
    respDesc.yDescriptionLength = yDescBytes.size();
    respDesc.xUnitLength = xUnitBytes.size();
    respDesc.yUnitLength = yUnitBytes.size();
    respDesc.datapointsLength = item.datapoints.size();

    WRITE_CHECKED_RAW(socket, respDesc);
    WRITE_CHECKED(socket, nameBytes);
    WRITE_CHECKED(socket, pathBytes);
    WRITE_CHECKED(socket, xDescBytes);
    WRITE_CHECKED(socket, yDescBytes);
    WRITE_CHECKED(socket, xUnitBytes);
    WRITE_CHECKED(socket, yUnitBytes);

   for (const auto &dp : item.datapoints) {
     IPCSockDatapoint respDp;

     respDp.x = std::get<0>(dp);
     respDp.y = std::get<1>(dp);

     WRITE_CHECKED_RAW(socket, respDp);
   }
  }
  FINALIZE(socket);
}

bool LocalSocketIPCProxy::respondSupportedFormats(QLocalSocket *socket)
{
  const QVector<FileFormatInfo> ffiVec = m_loader->supportedFileFormats();

  IPCSockResponseHeader responseHeader;
  INIT_RESPONSE(responseHeader, IPCSockResponseType::RESPONSE_SUPPORTED_FORMAT_HEADER, IPCS_SUCCESS);
  responseHeader.items = ffiVec.size();

  WRITE_CHECKED_RAW(socket, responseHeader);

  for (const auto &ffi : ffiVec) {
    IPCSockSupportedFormatResponseDescriptor desc;
    INIT_RESPONSE(desc, IPCSockResponseType::RESPONSE_SUPPORTED_FORMAT_DESCRIPTOR, IPCS_SUCCESS);

    QByteArray longDescription = ffi.longDescription.toUtf8();
    QByteArray shortDescription = ffi.shortDescription.toUtf8();
    QByteArray tag = ffi.tag.toUtf8();

    desc.longDescriptionLength = longDescription.size();
    desc.shortDescriptionLength = shortDescription.size();
    desc.tagLength = tag.size();
    if (ffi.loadOptions.size() > 1)
      desc.loadOptionsLength = ffi.loadOptions.size();
    else
      desc.loadOptionsLength = 0;

    /* Send the response descriptor */
    WRITE_CHECKED_RAW(socket, desc);

    /* Send the actual payload */
    WRITE_CHECKED(socket, longDescription);
    WRITE_CHECKED(socket, shortDescription);
    WRITE_CHECKED(socket, tag);

    if (ffi.loadOptions.size() > 1) {
      for (const QString &option : ffi.loadOptions) {
        IPCSockLoadOptionDescriptor loDesc;
        INIT_RESPONSE(loDesc, IPCSockResponseType::RESPONSE_LOAD_OPTION_DESCRIPTOR, IPCS_SUCCESS);

        QByteArray optionBA = option.toUtf8();
        loDesc.optionLength = optionBA.size();
        WRITE_CHECKED_RAW(socket, loDesc);

        WRITE_CHECKED(socket, optionBA);
      }
    }
  }
  FINALIZE(socket);
}
