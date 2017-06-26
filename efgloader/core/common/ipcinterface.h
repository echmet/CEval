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

class IPCDBusLoadOptionsVec : public QVector<QString>
{
  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusLoadOptionsVec &vec)
  {
    argument.beginArray(qMetaTypeId<QString>());
    for (const auto &item : vec)
      argument << item;
    argument.endArray();

    return argument;
  }

  friend const  QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusLoadOptionsVec &vec)
  {
    argument.beginArray();
    while (!argument.atEnd()) {
      QString d;
      argument >> d;
      vec.append(d);
    }
    argument.endArray();

    return argument;
  }
};
Q_DECLARE_METATYPE(IPCDBusLoadOptionsVec)

class IPCDBusSupportedFileFormat {
public:
  QString longDescription;
  QString shortDescription;
  QString tag;
  IPCDBusLoadOptionsVec loadOptions;

  friend QDBusArgument & operator<<(QDBusArgument &argument, const IPCDBusSupportedFileFormat &format)
  {
    argument.beginStructure();
    argument << format.longDescription;
    argument << format.shortDescription;
    argument << format.tag;
    argument << format.loadOptions;
    argument.endStructure();

    return argument;
  }

  friend const QDBusArgument & operator>>(const QDBusArgument &argument, IPCDBusSupportedFileFormat &format)
  {
    argument.beginStructure();
    argument >> format.longDescription;
    argument >> format.shortDescription;
    argument >> format.tag;
    argument >> format.loadOptions;
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
    qRegisterMetaType<IPCDBusLoadOptionsVec>("IPCDBusLoadOptionsVec");
    qDBusRegisterMetaType<IPCDBusLoadOptionsVec>();
    qRegisterMetaType<IPCDBusSupportedFileFormat>("IPCDBusSupportedFileFormat");
    qDBusRegisterMetaType<IPCDBusSupportedFileFormat>();
    qRegisterMetaType<IPCDBusSupportedFileFormatVec>("IPCDBusSupportedFileFormatVec");
    qDBusRegisterMetaType<IPCDBusSupportedFileFormatVec>();
  }
};

#endif // ENABLE_IPC_INTERFACE_DBUS

#ifdef CEVAL_COMPILER_MSVC
  #define PACKED_STRUCT \
    __pragma(pack()) struct
#elif defined CEVAL_COMPILER_GCC_LIKE
  #define PACKED_STRUCT \
    struct  __attribute__((packed))
#endif

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
  RESPONSE_LOAD_DATA_DESCRIPTOR = 0x4,
  RESPONSE_LOAD_OPTION_DESCRIPTOR = 0x5
};

enum IPCSocketLoadDataMode {
  IPCS_LOAD_INTERACTIVE = 0x1,
  IPCS_LOAD_HINT = 0x2,
  IPCS_LOAD_FILE = 0x3
};

PACKED_STRUCT IPCSockRequestHeader {
  uint16_t magic;
  uint8_t requestType;
};

PACKED_STRUCT IPCSockResponseHeader {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  int32_t items;
  uint32_t errorLength;
};

PACKED_STRUCT IPCSockLoadDataRequestDescriptor {
  uint16_t magic;
  uint8_t requestType;
  uint8_t mode;

  int32_t loadOption;
  uint32_t tagLength;
  uint32_t filePathLength;
};

PACKED_STRUCT IPCSockSupportedFormatResponseDescriptor {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  uint32_t longDescriptionLength;
  uint32_t shortDescriptionLength;
  uint32_t tagLength;
  uint32_t loadOptionsLength;
};

PACKED_STRUCT IPCSockLoadOptionDescriptor {
  uint16_t magic;
  uint8_t responseType;
  uint8_t status;

  uint32_t optionLength;
};

PACKED_STRUCT IPCSockLoadDataResponseDescriptor {
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

PACKED_STRUCT IPCSockDatapoint {
  double x;
  double y;
};

#endif // IPCINTERFACE_H
