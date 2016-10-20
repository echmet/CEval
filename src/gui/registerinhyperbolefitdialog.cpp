#include "registerinhyperbolefitdialog.h"
#include "ui_registerinhyperbolefitdialog.h"

RegisterInHyperboleFitDialog::RegisterInHyperboleFitDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RegisterInHyperboleFitDialog)
{
  ui->setupUi(this);

  m_registerInHFWidget = new RegisterInHyperboleFitWidget(this);

  this->layout()->addWidget(m_registerInHFWidget);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &RegisterInHyperboleFitDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &RegisterInHyperboleFitDialog::onOkClicked);
}

RegisterInHyperboleFitDialog::~RegisterInHyperboleFitDialog()
{
  delete ui;
}

RegisterInHyperboleFitWidget::MobilityFrom RegisterInHyperboleFitDialog::mobilityFrom() const
{
  return m_registerInHFWidget->mobilityFrom();
}

void RegisterInHyperboleFitDialog::onCancelClicked()
{
  reject();
}

void RegisterInHyperboleFitDialog::onOkClicked()
{
  accept();
}

void RegisterInHyperboleFitDialog::setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility)
{
  m_registerInHFWidget->setInformation(analyte, selConcentration, hvlMobility, peakMaxMobility);
}
