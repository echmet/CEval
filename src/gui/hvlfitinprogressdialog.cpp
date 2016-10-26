#include "hvlfitinprogressdialog.h"
#include "ui_hvlfitinprogressdialog.h"

HVLFitInProgressDialog::HVLFitInProgressDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLFitInProgressDialog),
  m_execable(true)
{
  ui->setupUi(this);

  connect(ui->qpb_abort, &QPushButton::clicked, this, &HVLFitInProgressDialog::onAbortClicked);
}

HVLFitInProgressDialog::~HVLFitInProgressDialog()
{
  delete ui;
}

int HVLFitInProgressDialog::exec()
{
  if (m_execable)
    return QDialog::exec();

  return QDialog::Accepted;
}

void HVLFitInProgressDialog::onAbortClicked()
{
  emit abortFit();
}

void HVLFitInProgressDialog::onHvlFitDone()
{
  m_execable = false;
  accept();
}

void HVLFitInProgressDialog::reject()
{
  return;
}
