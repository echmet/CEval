#include "softwareupdatewidget.h"
#include "ui_softwareupdatewidget.h"
#include "../globals.h"
#include "../softwareupdateresult.h"

SoftwareUpdateWidget::SoftwareUpdateWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SoftwareUpdateWidget)
{
  ui->setupUi(this);

  ui->ql_extraInfo->setText("");

  const QString currentVersionTag = QString("%1.%2%3").arg(Globals::VERSION_MAJ).arg(Globals::VERSION_MIN).arg(Globals::VERSION_REV);
  ui->ql_result->setText(tr("Check was not performed yet"));
  ui->ql_currentVersion->setText(currentVersionTag);
  ui->ql_newVersion->setText("");
  ui->ql_link->setText("");
}

SoftwareUpdateWidget::~SoftwareUpdateWidget()
{
  delete ui;
}

void SoftwareUpdateWidget::setDisplay(const SoftwareUpdateResult &result)
{
  ui->ql_newVersion->setText("");
  ui->ql_link->setText("");
  ui->ql_link->setOpenExternalLinks(false);
  ui->ql_extraInfo->setText("");

  switch (result.state) {
  case SoftwareUpdateResult::State::CHECK_ERROR:
    ui->ql_result->setText(QString(tr("Error occured during update check")));
    ui->ql_extraInfo->setText(result.error);
    return;
  case SoftwareUpdateResult::State::NETWORK_ERROR:
    ui->ql_result->setText(QString(tr("Network error occured during update check")));
    ui->ql_extraInfo->setText(result.error);
    return;
  case SoftwareUpdateResult::State::NO_DATA:
    ui->ql_result->setText(QString(tr("Update check did not return any data")));
    ui->ql_extraInfo->setText(result.error);
    return;
  case SoftwareUpdateResult::State::OK:
    if (!result.updateAvailable)
      ui->ql_result->setText(QString(tr("%1 is up to date")).arg(Globals::SOFTWARE_NAME));
    else {
      ui->ql_result->setText(QString(tr("Update for %1 is available")).arg(Globals::SOFTWARE_NAME));
      ui->ql_newVersion->setText(QString("%1.%2%3").arg(result.verMajor).arg(result.verMinor).arg(result.revision));
      ui->ql_link->setText(QString("Download new version from:<br /><a href=\"%1\">%1</a>").arg(result.downloadLink.toHtmlEscaped()));
      ui->ql_link->setOpenExternalLinks(true);

      switch (result.severity) {
      case SoftwareUpdateResult::Severity::AVAILABLE:
        ui->ql_extraInfo->setText(tr("New version contains new features or minor polishing. Update is recommended but not necessary."));
        break;
      case SoftwareUpdateResult::Severity::RECOMMENDED:
        ui->ql_extraInfo->setText(tr("New version contains fixes for issues likely to cause trouble during stanard operation of the software. Update is highly recommended."));
        break;
      case SoftwareUpdateResult::Severity::REQUIRED:
        ui->ql_extraInfo->setText(
              QString(tr("New version contains fixes for critical problems that may lead to incorrect calculations or other major malfunction. "
                         "%1 must be updated immediately.")).arg(Globals::SOFTWARE_NAME));
        break;
      }
    }
    return;
  case SoftwareUpdateResult::State::DISABLED:
    ui->ql_result->setText(tr("Updates are disabled in development versions"));
    return;
  }
}
