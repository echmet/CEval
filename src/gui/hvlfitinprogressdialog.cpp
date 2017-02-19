#include "hvlfitinprogressdialog.h"
#include "ui_hvlfitinprogressdialog.h"

HVLFitInProgressDialog::HVLFitInProgressDialog(const int maxIterations, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLFitInProgressDialog),
  m_execable(true),
  m_maxIterations(maxIterations)
{
  ui->setupUi(this);

  setCurrentIterationText(1);

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

void HVLFitInProgressDialog::setCurrentIteration(const int iteration)
{
  setCurrentIterationText(iteration);
}

void HVLFitInProgressDialog::setCurrentIterationText(const int iteration)
{
  ui->ql_iterations->setText(QString(tr("Iteration %1 of %2")).arg(iteration).arg(m_maxIterations));
}
