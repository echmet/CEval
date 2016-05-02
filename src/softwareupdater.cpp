#include "softwareupdater.h"
#include "globals.h"
#include "gui/autoupdatecheckdialog.h"

const QUrl SoftwareUpdater::UPDATE_LINK("http://devoid-pointer.net/echmet/testfile.xml");

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

SoftwareUpdater::SoftwareUpdater(QObject *parent) : QObject(parent),
  m_checkAutomatically(true)
{
  m_autoDlg = new AutoUpdateCheckDialog();

  connect(&m_fetcher, &UpdateListFetcher::listFetched, this, &SoftwareUpdater::onListFetched);
  connect(this, &SoftwareUpdater::automaticCheckComplete, this, &SoftwareUpdater::onAutomaticCheckComplete);
}

SoftwareUpdater::~SoftwareUpdater()
{
  m_fetcher.abortFetch();
}

void SoftwareUpdater::abortCheck()
{
  m_fetcher.abortFetch();
}

void SoftwareUpdater::checkAutomatically()
{
  if (!m_checkAutomatically)
    return;

  checkForUpdate(true);
}

void SoftwareUpdater::checkForUpdate(const bool automatic)
{
  UpdateListFetcher::RetCode tRet = m_fetcher.fetch(UPDATE_LINK);

  switch (tRet) {
  case UpdateListFetcher::RetCode::E_NOT_CONNECTED:
    if (!automatic)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Network connection is not available.")));
    break;
  case UpdateListFetcher::RetCode::E_IN_PROGRESS:
    if (!automatic)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Another check for update is already running.")));
    break;
  case UpdateListFetcher::RetCode::E_NO_MEM:
    if (!automatic)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("Insufficient memory.")));
    break;
  default:
    break;
  }

  m_automatic = automatic;
}

void SoftwareUpdater::onAutomaticCheckComplete(const UpdateCheckResults &results)
{
  SoftwareUpdateWidget::Result r;

  switch (results.status) {
  case UpdateCheckResults::Status::FAILED:
    r = SoftwareUpdateWidget::Result::FAILED;
    break;
  case UpdateCheckResults::Status::UP_TO_DATE:
    r = SoftwareUpdateWidget::Result::UP_TO_DATE;
    break;
  case UpdateCheckResults::Status::UPDATE_AVAILABLE:
    r = SoftwareUpdateWidget::Result::UPDATE_AVAILABLE;
    break;
  default:
    return;
  }

  m_autoDlg->setDisplay(r, results.versionTag, results.downloadLink);
  m_autoDlg->exec();
}

void SoftwareUpdater::onListFetched(const UpdateListFetcher::RetCode tRet, const SoftwareUpdateInfoMap &map)
{
  if (!m_automatic) {
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
    if (!m_automatic)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::FAILED, "", "", tr("No information about available updates were received.")));
    return;
  }

  const SoftwareUpdateInfo &info = map[swNameLower];
  const SoftwareUpdateInfo::Version &version = info.version;
  const SoftwareUpdateInfo::Version myVersion(Globals::VERSION_MAJ, Globals::VERSION_MIN, Globals::VERSION_REV);

  if (myVersion >= version) {
    if (!m_automatic)
      emit checkComplete(UpdateCheckResults(UpdateCheckResults::Status::UP_TO_DATE, "", "", ""));
  } else {
    QString versionTag = QString("%1.%2%3").arg(version.major).arg(version.minor).arg(version.revision);

    const UpdateCheckResults r(UpdateCheckResults::Status::UPDATE_AVAILABLE, info.downloadLink, versionTag, "");

    if (m_automatic)
      emit automaticCheckComplete(r);
    else
      emit checkComplete(r);
  }
}

void SoftwareUpdater::onSetAutoUpdate(const bool enabled)
{
  m_checkAutomatically = enabled;
}
