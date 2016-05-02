#include "softwareupdatewidget.h"
#include "ui_softwareupdatewidget.h"
#include "../globals.h"

SoftwareUpdateWidget::SoftwareUpdateWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SoftwareUpdateWidget)
{
  ui->setupUi(this);

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

void SoftwareUpdateWidget::setDisplay(const Result result, const QString &versionTag, const QString &downloadLink)
{
  ui->ql_newVersion->setText("");
  ui->ql_link->setText("");
  ui->ql_link->setOpenExternalLinks(false);

  switch (result) {
  case Result::FAILED:
    ui->ql_result->setText(tr("Check for updates has failed"));
    return;
    break;
  case Result::UP_TO_DATE:
    ui->ql_result->setText(QString(tr("%1 is up to date")).arg(Globals::SOFTWARE_NAME));
    return;
    break;
  case Result::UPDATE_AVAILABLE:
    ui->ql_result->setText(QString(tr("Update for %1 is available")).arg(Globals::SOFTWARE_NAME));
    break;
  }

  ui->ql_newVersion->setText(versionTag);
  ui->ql_link->setText(QString("Download new version from:<br /><a href=\"%1\">%1</a>").arg(downloadLink.toHtmlEscaped()));
  ui->ql_link->setOpenExternalLinks(true);
}
