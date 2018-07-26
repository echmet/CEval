#include "autoupdatecheckdialog.h"
#include "ui_autoupdatecheckdialog.h"

AutoUpdateCheckDialog::AutoUpdateCheckDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AutoUpdateCheckDialog)
{
  ui->setupUi(this);

  connect(ui->qcb_checkOnStartup, &QCheckBox::clicked, this, &AutoUpdateCheckDialog::onCheckOnStartupClicked);
  connect(ui->qpb_close, &QPushButton::clicked, this, &AutoUpdateCheckDialog::onCloseClicked);
}

AutoUpdateCheckDialog::~AutoUpdateCheckDialog()
{
  delete ui;
}

void AutoUpdateCheckDialog::onCheckOnStartupClicked()
{
  emit setAutoUpdate(ui->qcb_checkOnStartup->checkState() == Qt::Checked);
}

void AutoUpdateCheckDialog::onCloseClicked()
{
  close();
}

void AutoUpdateCheckDialog::setDisplay(const SoftwareUpdateResult &result)
{
  ui->qw_result->setDisplay(result);
}
