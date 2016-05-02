#include "globals.h"
#include "softwareupdater.h"

const QUrl SoftwareUpdater::UPDATE_LINK("http://devoid-pointer.net/echmet/xtestfile.xml");

UpdateCheckResults::UpdateCheckResults() :
  status(Status::INVALID),
  downloadLink(""),
  errorMessage(""),
  versionTag("")
{
}

UpdateCheckResults::UpdateCheckResults(const Status status, const QString downloadLink, const QString versionTag, const QString errorMessage) :
  status(status),
  downloadLink(downloadLink),
  errorMessage(errorMessage),
  versionTag(versionTag)
{
}

SoftwareUpdater::SoftwareUpdater(QObject *parent) : QObject(parent)
{
  connect(&m_fetcher, &UpdateListFetcher::listFetched, this, &SoftwareUpdater::onListFetched);
}

SoftwareUpdater::~SoftwareUpdater()
{
  m_fetcher.abortFetch();
}

void SoftwareUpdater::abortCheck()
{
  m_fetcher.abortFetch();
}

void SoftwareUpdater::checkForUpdate(const bool silent)
{
  UpdateListFetcher::RetCode tRet = m_fetcher.fetch(UPDATE_LINK);

  switch (tRet) {
  case UpdateListFetcher::RetCode::E_NOT_CONNECTED:
    if (!silent)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Network connection is not available.")));
    break;
  case UpdateListFetcher::RetCode::E_IN_PROGRESS:
    if (!silent)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Another check for update is already running.")));
    break;
  case UpdateListFetcher::RetCode::E_NO_MEM:
    if (!silent)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Insufficient memory.")));
    break;
  default:
    break;
  }

  m_silent = silent;
}

void SoftwareUpdater::onListFetched(const UpdateListFetcher::RetCode tRet, const SoftwareUpdateInfoMap &map)
{
  if (!m_silent) {
    switch (tRet) {
    case UpdateListFetcher::RetCode::E_INVALID_FILE_STRUCTURE:
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Invalid update info received.")));
      return;
      break;
    case UpdateListFetcher::RetCode::E_MALFORMED_XML:
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Malformed XML document received.")));
      return;
      break;
    case UpdateListFetcher::RetCode::E_NETWORK_ERROR:
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("A network error occured while fetching update information. Please try again later.")));
      return;
      break;
    default:
      break;
    }
  }

  const QString swNameLower = Globals::SOFTWARE_NAME.toLower();
  if (!map.contains(swNameLower)) {
    emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("No information about available updates were received.")));
    return;
  }

  const SoftwareUpdateInfo &info = map[swNameLower];
  const SoftwareUpdateInfo::Version &version = info.version;
  const SoftwareUpdateInfo::Version myVersion(Globals::VERSION_MAJ, Globals::VERSION_MIN, Globals::VERSION_REV);

  if (myVersion >= version)
    emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::UP_TO_DATE, "", "", ""));
  else {
    QString versionTag = QString("%1.%2%3").arg(version.major).arg(version.minor).arg(version.revision);
    emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::UPDATE_AVAILABLE, info.downloadLink, versionTag, ""));
  }
}
