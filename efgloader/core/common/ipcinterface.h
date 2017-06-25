#ifndef IPCINTERFACE_H
#define IPCINTERFACE_H

#ifdef ENABLE_IPC_INTERFACE_DBUS

#define DBUS_SERVICE_NAME "cz.cuni.natur.echmet.ceval.dataloader"
#define DBUS_OBJECT_PATH "/DataLoader"

#include <QObject>
#include <QVector>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>

class IPCDBusDatapoint {
public:
  double x;
  double y;

  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusDatapoint &result)
  {
    argument.beginStructure();
    argument << result.x;
    argument << result.y;
    argument.endStructure();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusDatapoint &result)
  {
    argument.beginStructure();
    argument >> result.x;
    argument >> result.y;
    argument.endStructure();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusDatapoint)

class IPCDBusData {
public:
  QString path;
  QString name;

  QString xDescription;
  QString yDescription;
  QString xUnit;
  QString yUnit;

  QVector<IPCDBusDatapoint> datapoints;

  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusData &result)
  {
    argument.beginStructure();
    argument << result.path;
    argument << result.name;
    argument << result.xDescription;
    argument << result.yDescription;
    argument << result.xUnit;
    argument << result.yUnit;
    argument << result.datapoints;
    argument.endStructure();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusData &result)
  {
    argument.beginStructure();
    argument >> result.path;
    argument >> result.name;
    argument >> result.xDescription;
    argument >> result.yDescription;
    argument >> result.xUnit;
    argument >> result.yUnit;
    argument >> result.datapoints;
    argument.endStructure();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusData)

class IPCDBusDataVec : public QVector<IPCDBusData> {
  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusDataVec &vec)
  {
    argument.beginArray(qMetaTypeId<IPCDBusData>());
    for (const auto &item : vec)
      argument << item;
    argument.endArray();

    return argument;
  }

  friend const  QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusDataVec &vec)
  {
    argument.beginArray();
    while (!argument.atEnd()) {
      IPCDBusData d;
      argument >> d;
      vec.append(d);
    }
    argument.endArray();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusDataVec)

class IPCDBusDataPack {
public:
  bool success;
  QString error;
  IPCDBusDataVec data;

  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusDataPack &pack)
  {
    argument.beginStructure();
    argument << pack.success;
    argument << pack.error;
    argument << pack.data;
    argument.endStructure();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusDataPack &pack)
  {
    argument.beginStructure();
    argument >> pack.success;
    argument >> pack.error;
    argument >> pack.data;
    argument.endStructure();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusDataPack)

class IPCDBusSupportedFileFormat {
public:
  QString longDescription;
  QString shortDescription;
  QString tag;

  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusSupportedFileFormat &format)
  {
    argument.beginStructure();
    argument << format.longDescription;
    argument << format.shortDescription;
    argument << format.tag;
    argument.endStructure();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusSupportedFileFormat &format)
  {
    argument.beginStructure();
    argument >> format.longDescription;
    argument >> format.shortDescription;
    argument >> format.tag;
    argument.endStructure();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusSupportedFileFormat)

class IPCDBusSupportedFileFormatVec : public QVector<IPCDBusSupportedFileFormat>
{
  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusSupportedFileFormatVec &vec)
  {
    argument.beginArray(qMetaTypeId<IPCDBusSupportedFileFormat>());
    for (const auto &item : vec)
      argument << item;

    argument.endArray();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusSupportedFileFormatVec &vec)
  {
    argument.beginArray();

    while (!argument.atEnd()) {
      IPCDBusSupportedFileFormat sff;
      argument >> sff;
      vec.append(sff);
    }
    argument.endArray();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusSupportedFileFormatVec)

class DBusMetyTypesRegistrator {
public:
  static void registerAll()
  {
    qRegisterMetaType<IPCDBusDatapoint>("IPCDBusDatapoint");
    qDBusRegisterMetaType<IPCDBusDatapoint>();
    qRegisterMetaType<IPCDBusData>("IPCDBusData");
    qDBusRegisterMetaType<IPCDBusData>();
    qRegisterMetaType<IPCDBusDataVec>("IPCDBusDataVec");
    qDBusRegisterMetaType<IPCDBusDataVec>();
    qRegisterMetaType<IPCDBusDataPack>("IPCDBusDataPack");
    qDBusRegisterMetaType<IPCDBusDataPack>();
    qRegisterMetaType<IPCDBusSupportedFileFormat>("IPCDBusSupportedFileFormat");
    qDBusRegisterMetaType<IPCDBusSupportedFileFormat>();
    qRegisterMetaType<IPCDBusSupportedFileFormatVec>("IPCDBusSupportedFileFormatVec");
    qDBusRegisterMetaType<IPCDBusSupportedFileFormatVec>();
  }
};

#endif // ENABLE_IPC_INTERFACE_DBUS

#include <stdint.h>

#define IPCS_PACKET_MAGIC 0x091E
#define IPCS_SOCKET_NAME "cevalefgloader"

enum IPCSockRequestType {
  REQUEST_SUPPORTED_FORMATS = 0x1,
  REQUEST_LOAD_DATA = 0x2,
  REQUEST_LOAD_DATA_DESCRIPTOR = 0x3,
};

enum IPCSockResult {
  IPCS_SUCCESS = 0x1,
  IPCS_FAILURE = 0x2
};

enum IPCSockResponseType {
  RESPONSE_SUPPORTED_FORMAT_HEADER = 0x1,
  RESPONSE_LOAD_DATA_HEADER = 0x2,
  RESPONSE_SUPPORTED_FORMAT_DESCRIPTOR = 0x3,
  RESPONSE_LOAD_DATA_DESCRIPTOR = 0x4
};

enum IPCSocketLoadDataMode {
  IPCS_LOAD_INTERACTIVE,
  IPCS_LOAD_HINT,
  IPCS_LOAD_FILE
};

struct __attribute__((packed)) IPCSockRequestHeader {
  uint16_t magic;
  uint8_t requestType;
};

struct __attribute__((packed)) IPCSockResponseHeader {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  int32_t items;
  uint32_t errorLength;
};

struct __attribute__((packed)) IPCSockLoadDataRequestDescriptor {
  uint16_t magic;
  uint8_t requestType;
  uint8_t mode;

  uint32_t tagLength;
  uint32_t filePathLength;
};

struct __attribute__((packed)) IPCSockSupportedFormatResponseDescriptor {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  uint32_t longDescriptionLength;
  uint32_t shortDescriptionLength;
  uint32_t tagLength;
};

struct __attribute__((packed)) IPCSockLoadDataResponseDescriptor {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  uint32_t nameLength;
  uint32_t pathLength;
  uint32_t xDescriptionLength;
  uint32_t yDescriptionLength;
  uint32_t xUnitLength;
  uint32_t yUnitLength;
  uint32_t datapointsLength;
};

struct __attribute__((packed)) IPCSockDatapoint {
  double x;
  double y;
};

#endif // IPCINTERFACE_H
