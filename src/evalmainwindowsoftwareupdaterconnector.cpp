#include "gui/evalmainwindow.h"
#include "gui/checkforupdatedialog.h"
#include "softwareupdater.h"

void EvalMainWindow::connectToSoftwareUpdater(SoftwareUpdater *updater)
{
  m_updateDlg->setCheckOnStartup(updater->automaticCheckEnabled());

  connect(updater, &SoftwareUpdater::checkComplete, m_updateDlg, &CheckForUpdateDialog::onCheckComplete);
  connect(m_updateDlg, &CheckForUpdateDialog::checkForUpdate, updater, &SoftwareUpdater::onCheckForUpdate);
  connect(m_updateDlg, &CheckForUpdateDialog::setAutoUpdate, updater, &SoftwareUpdater::onSetAutoUpdate);
  connect(m_updateDlg, &CheckForUpdateDialog::closed, updater, &SoftwareUpdater::abortCheck);
  connect(updater, &SoftwareUpdater::autoUpdateChanged, m_updateDlg, &CheckForUpdateDialog::onAutoUpdateChanged);
}
