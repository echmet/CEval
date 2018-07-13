#include "hvlestimateinprogressdialog.h"
#include "ui_hvlestimateinprogressdialog.h"

HVLEstimateInProgressDialog::HVLEstimateInProgressDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLEstimateInProgressDialog)
{
  ui->setupUi(this);
}

HVLEstimateInProgressDialog::~HVLEstimateInProgressDialog()
{
  delete ui;
}

void HVLEstimateInProgressDialog::onEstimateDone()
{
  close();
}
