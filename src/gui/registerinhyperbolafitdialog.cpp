#include "registerinhyperbolafitdialog.h"
#include "ui_registerinhyperbolafitdialog.h"

RegisterInHyperbolaFitDialog::RegisterInHyperbolaFitDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RegisterInHyperbolaFitDialog)
{
  ui->setupUi(this);

  m_registerInHFWidget = new RegisterInHyperbolaFitWidget(this);

  ui->qw_container->setLayout(new QVBoxLayout());
  ui->qw_container->layout()->addWidget(m_registerInHFWidget);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &RegisterInHyperbolaFitDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &RegisterInHyperbolaFitDialog::onOkClicked);
}

RegisterInHyperbolaFitDialog::~RegisterInHyperbolaFitDialog()
{
  delete ui;
}

RegisterInHyperbolaFitWidget::MobilityFrom RegisterInHyperbolaFitDialog::mobilityFrom() const
{
  return m_registerInHFWidget->mobilityFrom();
}

void RegisterInHyperbolaFitDialog::onCancelClicked()
{
  reject();
}

void RegisterInHyperbolaFitDialog::onOkClicked()
{
  accept();
}

void RegisterInHyperbolaFitDialog::setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility)
{
  m_registerInHFWidget->setInformation(analyte, selConcentration, hvlMobility, peakMaxMobility);
}
