#include "autoupdatecheckdialog.h"
#include "ui_autoupdatecheckdialog.h"

AutoUpdateCheckDialog::AutoUpdateCheckDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AutoUpdateCheckDialog)
{
  ui->setupUi(this);
}

AutoUpdateCheckDialog::~AutoUpdateCheckDialog()
{
  delete ui;
}
