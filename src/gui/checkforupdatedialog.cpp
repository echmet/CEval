#include "checkforupdatedialog.h"
#include "ui_checkforupdatedialog.h"
#include "../updatecheckresults.h"
#include "softwareupdatewidget.h"

CheckForUpdateDialog::CheckForUpdateDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CheckForUpdateDialog)
{
  ui->setupUi(this);

  connect(ui->qcb_checkOnStartup, &QCheckBox::clicked, this, &CheckForUpdateDialog::onCheckOnStartupClicked);
  connect(ui->qpb_check, &QPushButton::clicked, this, &CheckForUpdateDialog::onCheckNowClicked);
  connect(ui->qpb_close, &QPushButton::clicked, this, &CheckForUpdateDialog::onCloseClicked);
}

CheckForUpdateDialog::~CheckForUpdateDialog()
{
  delete ui;
}

void CheckForUpdateDialog::closeEvent(QCloseEvent *ev)
{
  Q_UNUSED(ev);

  emit closed();
}

void CheckForUpdateDialog::onAutoUpdateChanged(const bool enabled)
{
  ui->qcb_checkOnStartup->setChecked(enabled);
}

void CheckForUpdateDialog::onCheckComplete(const UpdateCheckResults &results)
{
  SoftwareUpdateWidget::Result r;

  switch (results.status) {
  case UpdateCheckResults::Status::FAILED:
    r = SoftwareUpdateWidget::Result::FAILED;
    ui->ql_error->setText(results.errorMessage);
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

  ui->qw_result->setDisplay(r, results.versionTag, results.downloadLink);
}

void CheckForUpdateDialog::onCheckNowClicked()
{
  emit checkForUpdate();
}

void CheckForUpdateDialog::onCheckOnStartupClicked()
{
  emit setAutoUpdate(ui->qcb_checkOnStartup->checkState() == Qt::Checked);
}

void CheckForUpdateDialog::onCloseClicked()
{
  close();
}

void CheckForUpdateDialog::setCheckOnStartup(const bool checked)
{
  ui->qcb_checkOnStartup->setChecked(checked);
}
