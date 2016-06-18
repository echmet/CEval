#include "hvlfitinprogressdialog.h"
#include "ui_hvlfitinprogressdialog.h"

HVLFitInProgressDialog::HVLFitInProgressDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLFitInProgressDialog)
{
  ui->setupUi(this);

  connect(ui->qpb_abort, &QPushButton::clicked, this, &HVLFitInProgressDialog::onAbortClicked);
}

HVLFitInProgressDialog::~HVLFitInProgressDialog()
{
  delete ui;
}

void HVLFitInProgressDialog::onAbortClicked()
{
  emit abortFit();
}

void HVLFitInProgressDialog::onHvlFitDone()
{
  accept();
}

void HVLFitInProgressDialog::reject()
{
  return;
}
