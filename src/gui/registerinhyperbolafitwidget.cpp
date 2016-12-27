#include "registerinhyperbolafitwidget.h"
#include "ui_registerinhyperbolafitwidget.h"
#include "../doubletostringconvertor.h"
#include <QComboBox>
#include <cmath>

RegisterInHyperbolaFitWidget::RegisterInHyperbolaFitWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::RegisterInHyperbolaFitWidget)
{
  ui->setupUi(this);

  ui->qcbox_mobilityFrom->addItem(tr("HVL a1 parameter"), QVariant::fromValue<MobilityFrom>(MobilityFrom::HVL_A1));
  ui->qcbox_mobilityFrom->addItem(tr("Peak maximum"), QVariant::fromValue<MobilityFrom>(MobilityFrom::PEAK_MAXIMUM));

  connect(ui->qcbox_mobilityFrom, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &RegisterInHyperbolaFitWidget::onMobilityFromActivated);
}

RegisterInHyperbolaFitWidget::~RegisterInHyperbolaFitWidget()
{
  delete ui;
}

RegisterInHyperbolaFitWidget::MobilityFrom RegisterInHyperbolaFitWidget::mobilityFrom() const
{
  return ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>();
}

void RegisterInHyperbolaFitWidget::onMobilityFromActivated(const int idx)
{
  Q_UNUSED(idx);

  setMobilityText(ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>());
}

void RegisterInHyperbolaFitWidget::setAnalyteText(const QString &text)
{
  ui->ql_analyteRes->setText(text);

  if (text.length() < 1) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->ql_analyte->setPalette(palette);
  } else {
    ui->ql_analyte->setPalette(QPalette());
  }
}

void RegisterInHyperbolaFitWidget::setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility)
{
  ui->ql_selConcRes->setText(QString::number(selConcentration));
  ui->ql_analyteRes->setText(analyte);

  m_hvlMobility = hvlMobility;
  m_peakMaxMobility = peakMaxMobility;

  setMobilityText(ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>());
}

void RegisterInHyperbolaFitWidget::setMobilityText(const MobilityFrom m)
{
  double dm;

  switch (m) {
  case MobilityFrom::HVL_A1:
    dm = m_hvlMobility;
    break;
  case MobilityFrom::PEAK_MAXIMUM:
    dm = m_peakMaxMobility;
    break;
  }

  if (!std::isfinite(dm)) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, Qt::red);

    ui->ql_mobility->setPalette(palette);
    ui->ql_mobilityRes->setPalette(palette);
    ui->ql_mobilityRes->setText("Invalid");
  } else {
    ui->ql_mobility->setPalette(QPalette());
    ui->ql_mobilityRes->setPalette(QPalette());

    ui->ql_mobilityRes->setText(DoubleToStringConvertor::convert(dm));
  }
}
