#include "updatelistfetcher.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QXmlStreamReader>

const char * UpdateListFetcher::DuplicitInformationException::what() const noexcept
{
  return "Software information list contains multiple information for one software";
}

UpdateListFetcher::InvalidDocumentStructureException::InvalidDocumentStructureException(const qint64 line) :
  line(line)
{
}

const char * UpdateListFetcher::InvalidDocumentStructureException::what() const noexcept
{
  return "Document has invalid structure";
}

UpdateListFetcher::UpdateListFetcher(QObject *parent) : QObject(parent),
  m_netReply(nullptr)
{
  m_netMgr = new QNetworkAccessManager(this);
  m_xmlReader = new QXmlStreamReader();
}

UpdateListFetcher::~UpdateListFetcher()
{
  delete m_xmlReader;
}

void UpdateListFetcher::abortFetch()
{
  m_netLock.lock();

  if (m_netReply != nullptr) {
    if (m_netReply->isRunning()) {
      m_netReply->abort();
      m_netReply->deleteLater();
    }
  }

  m_netLock.unlock();
}

UpdateListFetcher::RetCode UpdateListFetcher::fetch(const QUrl &link)
{
  RetCode tRet;
  QNetworkRequest request(link);

  m_netLock.lock();

  if (m_netReply != nullptr) {
    tRet = RetCode::E_IN_PROGRESS;
    goto out;
  }

  if (m_netMgr->networkAccessible() != QNetworkAccessManager::Accessible) {
    tRet = RetCode::E_NOT_CONNECTED;
    goto out;
  }

  m_netReply = m_netMgr->get(request);

  if (m_netReply == nullptr) {
    tRet =  RetCode::E_NO_MEM;
    goto out;
  }

  connect(m_netReply, &QNetworkReply::finished, this, &UpdateListFetcher::onDataAvailable);
  connect(m_netReply, &QNetworkReply::destroyed, this, &UpdateListFetcher::onReplyDeleted);

  tRet = RetCode::OK;

out:
  m_netLock.unlock();

  return tRet;
}

void UpdateListFetcher::onDataAvailable()
{
  m_netLock.lock();

  const QByteArray &data = m_netReply->readAll();
  RetCode tRet;

  if (m_netReply->error() != QNetworkReply::NoError) {
    tRet = RetCode::E_NETWORK_ERROR;
    goto out;
  }

  m_updateInfoList.clear();
  m_xmlReader->addData(data);
  m_parseState = ParseState::AT_START;

  try {
    tRet = parseInternal();
  } catch (InvalidDocumentStructureException &ex) {
    tRet = RetCode::E_INVALID_FILE_STRUCTURE;
  } catch (DuplicitInformationException &ex) {
    tRet = RetCode::E_INVALID_DATA;
  }

out:
  m_netReply->deleteLater();
  m_xmlReader->clear();

  m_netLock.unlock();

  emit listFetched(tRet, m_updateInfoList);
}

void UpdateListFetcher::onReplyDeleted()
{
  m_netReply = nullptr;
}

UpdateListFetcher::RetCode UpdateListFetcher::parseInternal()
{
  while (!m_xmlReader->atEnd()) {
    QXmlStreamReader::TokenType tt = m_xmlReader->readNext();

    switch (tt) {
    case QXmlStreamReader::StartDocument:
      if (m_parseState != ParseState::AT_START)
        throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

      m_parseState = ParseState::AT_ROOT;
      break;
    case QXmlStreamReader::StartElement:
    {
      const QStringRef name = m_xmlReader->qualifiedName();

      if (name.compare(QString("updateinfo")) == 0) {
        if (m_parseState != ParseState::AT_ROOT)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_ROOT;
      } else if (name.compare(QString("software")) == 0) {
        if (m_parseState != ParseState::IN_ROOT)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_SOFTWARE;
        resetSoftwareInfo();
      } else if (name.compare(QString("name")) == 0 ||
                 name.compare(QString("link")) == 0) {
        if (m_parseState != ParseState::IN_SOFTWARE)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_INFO;
      } else if (name.compare(QString("version")) == 0) {
        if (m_parseState != ParseState::IN_SOFTWARE)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_VERSION;
      } else if (name.compare(QString("major")) == 0 ||
                 name.compare(QString("minor")) == 0 ||
                 name.compare(QString("revision")) == 0) {
        if (m_parseState != ParseState::IN_VERSION)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_VERSION_INFO;
      } else {
        m_xmlReader->skipCurrentElement();
      }

      m_lastElementName = name.toString();

      break;
    }
    case QXmlStreamReader::Characters:
    {
      const QString &name = m_lastElementName;
      const QString text = *m_xmlReader->text().string();

      if (name.compare("name") == 0) {
        if (m_parseState != ParseState::IN_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_swName = text;
      } else if (name.compare("link") == 0) {
        if (m_parseState != ParseState::IN_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_swDLLink = text;
      } else if (name.compare("major") == 0) {
        if (m_parseState != ParseState::IN_VERSION_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_verMajorStr = text;
      } else if (name.compare("minor") == 0) {
        if (m_parseState != ParseState::IN_VERSION_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_verMinorStr = text;
      } else if (name.compare("revision") == 0) {
        if (m_parseState != ParseState::IN_VERSION_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_verRevStr = text;
      }

      break;
    }
    case QXmlStreamReader::EndElement:
    {
      const QStringRef name = m_xmlReader->qualifiedName();

      m_lastElementName = "";

      if (name.compare(QString("updateinfo")) == 0) {
        if (m_parseState != ParseState::IN_ROOT)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::AT_ROOT;
      } else if (name.compare(QString("software")) == 0) {
        if (m_parseState != ParseState::IN_SOFTWARE)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_ROOT;

        /* Check that we have a complete record */
        bool ok = false;

        if (m_swName.length() == 0)
          break;

        if (m_swDLLink.length() == 0)
          break;

        int major = m_verMajorStr.toInt(&ok);
        if (!ok) {
          break;
        }

        int minor = m_verMinorStr.toInt(&ok);
        if (!ok)
          break;

        try {
          SoftwareUpdateInfo sui(m_swName, SoftwareUpdateInfo::Version(major, minor, m_verRevStr), m_swDLLink);

          const QString swNameLower(m_swName.toLower());

          if (m_updateInfoList.contains(swNameLower))
            throw DuplicitInformationException();

          m_updateInfoList.insert(swNameLower, sui);
        } catch (SoftwareUpdateInfo::Version::InvalidRevisionStringException &) {
        }
      } else if (name.compare(QString("name")) == 0 ||
                 name.compare(QString("link")) == 0) {
        if (m_parseState != ParseState::IN_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_SOFTWARE;
      } else if (name.compare(QString("version")) == 0) {
        if (m_parseState != ParseState::IN_VERSION)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_SOFTWARE;
      } else if (name.compare(QString("major")) == 0 ||
                 name.compare(QString("minor")) == 0 ||
                 name.compare(QString("revision")) == 0) {
        if (m_parseState != ParseState::IN_VERSION_INFO)
          throw InvalidDocumentStructureException(m_xmlReader->lineNumber());

        m_parseState = ParseState::IN_VERSION;
      }

      break;
    }
    default:
      break;
    }
  }

  if (m_xmlReader->hasError())
    return RetCode::E_MALFORMED_XML;

  return RetCode::OK;
}

void UpdateListFetcher::resetSoftwareInfo()
{
  m_swDLLink = "";
  m_swName = "";
  m_verMajorStr = "";
  m_verMinorStr = "";
  m_verRevStr = "";
}
