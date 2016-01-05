#include "hvlfitinprogressdialog.h"
#include "ui_hvlfitinprogressdialog.h"

HVLFitInProgressDialog::HVLFitInProgressDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLFitInProgressDialog)
{
  ui->setupUi(this);
}

HVLFitInProgressDialog::~HVLFitInProgressDialog()
{
  delete ui;
}

void HVLFitInProgressDialog::onHvlFitDone()
{
  accept();
}

void HVLFitInProgressDialog::reject()
{
  return;
}
