#include "hvlfitinprogressdialog.h"
#include "ui_hvlfitinprogressdialog.h"
#include "../doubletostringconvertor.h"

HVLFitInProgressDialog::HVLFitInProgressDialog(const int maxIterations, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HVLFitInProgressDialog),
  m_execable(true),
  m_maxIterations(maxIterations)
{
  ui->setupUi(this);

  setCurrentIterationText(1, -1.0);

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

void HVLFitInProgressDialog::setCurrentIteration(const int iteration, const double avgTimePerIter)
{
  setCurrentIterationText(iteration, avgTimePerIter);
}

void HVLFitInProgressDialog::setCurrentIterationText(const int iteration, const double avgTimePerIter)
{
  ui->ql_iterations->setText(QString(tr("Iteration %1 of %2")).arg(iteration).arg(m_maxIterations));

  if (avgTimePerIter < 0.0)
    ui->ql_avgTimePerIterValue->setText("-");
  else
    ui->ql_avgTimePerIterValue->setText(DoubleToStringConvertor::convert(avgTimePerIter, 3));
}
