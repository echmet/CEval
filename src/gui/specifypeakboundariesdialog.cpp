#include "specifypeakboundariesdialog.h"
#include "ui_specifypeakboundariesdialog.h"
#include "../doubletostringconvertor.h"
#include <QMessageBox>

SpecifyPeakBoundariesDialog::Answer::Answer() :
  fromX(0.0),
  fromY(0.0),
  toX(0.0),
  toY(0.0),
  snapFrom(false),
  snapTo(false),
  m_valid(false)
{
}

SpecifyPeakBoundariesDialog::Answer::Answer(const double fromX, const double fromY, const double toX, const double toY,
                                            const bool snapFrom, const bool snapTo) :
  fromX(fromX),
  fromY(fromY),
  toX(toX),
  toY(toY),
  snapFrom(snapFrom),
  snapTo(snapTo),
  m_valid(true)
{
}

SpecifyPeakBoundariesDialog::Answer::Answer(const Answer &other) :
  fromX(other.fromX),
  fromY(other.fromY),
  toX(other.toX),
  toY(other.toY),
  snapFrom(other.snapFrom),
  snapTo(other.snapTo),
  m_valid(other.m_valid)
{
}

SpecifyPeakBoundariesDialog::Answer & SpecifyPeakBoundariesDialog::Answer::operator=(const Answer &other)
{
  const_cast<double&>(fromX) = other.fromX;
  const_cast<double&>(fromY) = other.fromY;
  const_cast<double&>(toX) = other.toX;
  const_cast<double&>(toY) = other.toY;
  const_cast<bool&>(snapFrom) = other.snapFrom;
  const_cast<bool&>(snapTo) = other.snapTo;
  const_cast<bool&>(m_valid) = other.m_valid;

  return *this;
}

bool SpecifyPeakBoundariesDialog::Answer::isValid() const
{
  return m_valid;
}

SpecifyPeakBoundariesDialog::SpecifyPeakBoundariesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SpecifyPeakBoundariesDialog)
{
  ui->setupUi(this);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &SpecifyPeakBoundariesDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &SpecifyPeakBoundariesDialog::onOkClicked);
  connect(ui->qcb_snapFrom, &QCheckBox::checkStateChanged, this, &SpecifyPeakBoundariesDialog::onSnapFromChanged);
  connect(ui->qcb_snapTo, &QCheckBox::checkStateChanged, this, &SpecifyPeakBoundariesDialog::onSnapToChanged);
}

SpecifyPeakBoundariesDialog::~SpecifyPeakBoundariesDialog()
{
  delete ui;
}

SpecifyPeakBoundariesDialog::Answer SpecifyPeakBoundariesDialog::answer()
{
  return m_answer;
}

void SpecifyPeakBoundariesDialog::onCancelClicked()
{
  reject();
}

void SpecifyPeakBoundariesDialog::onOkClicked()
{
  bool ok;
  double fromY;
  double toY;

  const double fromX = DoubleToStringConvertor::back(ui->qle_peakFromX->text(), &ok);
  if (!ok) {
    QMessageBox::warning(this, tr("Invalid input"), tr("Invalid data in \"Peak from X\""));
    return;
  }

  if (ui->qcb_snapFrom->checkState() != Qt::Checked) {
    fromY = DoubleToStringConvertor::back(ui->qle_peakFromY->text(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Invalid data in \"Peak from Y\""));
      return;
    }
  } else
    fromY = 0.0;

  const double toX = DoubleToStringConvertor::back(ui->qle_peakToX->text(), &ok);
  if (!ok) {
    QMessageBox::warning(this, tr("Invalid input"), tr("Invalid data in \"Peak to X\""));
    return;
  }

  if (ui->qcb_snapTo->checkState() != Qt::Checked) {
    toY = DoubleToStringConvertor::back(ui->qle_peakToY->text(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Invalid data in \"Peak to Y\""));
      return;
    }
  } else
    toY = 0.0;

  m_answer = Answer(fromX, fromY, toX, toY,
                    ui->qcb_snapFrom->checkState() == Qt::Checked, ui->qcb_snapTo->checkState() == Qt::Checked);

  accept();
}

void SpecifyPeakBoundariesDialog::onSnapFromChanged(const int state)
{
  if (state == Qt::Checked)
    ui->qle_peakFromY->setEnabled(false);
  else
    ui->qle_peakFromY->setEnabled(true);
}

void SpecifyPeakBoundariesDialog::onSnapToChanged(const int state)
{
  if (state == Qt::Checked)
    ui->qle_peakToY->setEnabled(false);
  else
    ui->qle_peakToY->setEnabled(true);
}
