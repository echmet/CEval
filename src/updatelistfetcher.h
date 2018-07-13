#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include "softwareupdateinfo.h"

class QNetworkAccessManager;
class QNetworkReply;
class QXmlStreamReader;

typedef QMap<QString, SoftwareUpdateInfo> SoftwareUpdateInfoMap;

class UpdateListFetcher : public QObject
{
  Q_OBJECT
public:
  enum class RetCode {
    OK,
    E_NETWORK_ERROR,
    E_IN_PROGRESS,
    E_INVALID_DATA,
    E_INVALID_FILE_STRUCTURE,
    E_MALFORMED_XML,
    E_NO_MEM,
    E_NOT_CONNECTED
  };
  Q_ENUM(RetCode)

  class DuplicitInformationException : public std::exception {
  public:
    const char * what() const noexcept;

  };

  class InvalidDocumentStructureException : public std::exception {
  public:
    InvalidDocumentStructureException(qint64 line);
    const char * what() const noexcept;

    const qint64 line;

  };

  UpdateListFetcher(QObject *parent = nullptr);
  ~UpdateListFetcher();
  void abortFetch();
  RetCode fetch(const QUrl &link);

private:
  enum class ParseState {
    AT_START,        /* Parser is outside the XML tree */
    AT_ROOT,         /* Parser is at the top of the XML tree */
    IN_ROOT,         /* Parser has descended into the <updateinfo> element */
    IN_SOFTWARE,     /* Parser has descended into the <software> element */
    IN_INFO,         /* Parser is inside one of the descriptor elements within the <software> tree */
    IN_VERSION,      /* Parser has descended into the <version> tree */
    IN_VERSION_INFO  /* Parser is inside one of the descriptor elements within the <version> tree */
  };

  RetCode parseInternal();
  void resetSoftwareInfo();

  QNetworkAccessManager *m_netMgr;
  QNetworkReply *m_netReply;
  QXmlStreamReader *m_xmlReader;

  QMutex m_netLock;

  SoftwareUpdateInfoMap m_updateInfoList;

  ParseState m_parseState;
  QString m_lastElementName;
  QString m_swName;
  QString m_swDLLink;
  QString m_verMajorStr;
  QString m_verMinorStr;
  QString m_verRevStr;

signals:
  void listFetched(const RetCode ret, const SoftwareUpdateInfoMap &map);

private slots:
  void onDataAvailable();
  void onReplyDeleted();

};

#endif // XMLPARSER_H
