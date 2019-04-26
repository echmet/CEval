#include "addpeakdialog.h"
#include "ui_addpeakdialog.h"
#include "../doubletostringconvertor.h"
#include <QCompleter>

AddPeakDialog::Answer::Answer(const bool registerInHF, const QString &name, const RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom) :
  registerInHF(registerInHF),
  name(name),
  mobilityFrom(mobilityFrom)
{
}

AddPeakDialog::AddPeakDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddPeakDialog)
{
  ui->setupUi(this);
  m_registerInHFWidget = new RegisterInHyperbolaFitWidget(this);
  ui->groupBox->layout()->addWidget(m_registerInHFWidget);

  ui->qcbox_analyte->setModel(&m_analytesModel);
  ui->qcbox_analyte->completer()->setCaseSensitivity(Qt::CaseSensitive);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &AddPeakDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &AddPeakDialog::onOkClicked);
  connect(ui->qcb_register, &QCheckBox::clicked, this, &AddPeakDialog::onRegisterClicked);
  connect(ui->qcbox_analyte, &QComboBox::currentTextChanged, this, &AddPeakDialog::onAnalyteTextChanged);

  onAnalyteTextChanged(ui->qcbox_analyte->currentText());
  onRegisterClicked();
}

AddPeakDialog::~AddPeakDialog()
{
  delete ui;
}

AddPeakDialog::Answer AddPeakDialog::answer() const
{
  return Answer(ui->qcb_register->checkState() == Qt::Checked,
                ui->qcbox_analyte->currentText(),
                m_registerInHFWidget->mobilityFrom()
                );
}

void AddPeakDialog::onAnalyteTextChanged(const QString &s)
{
  m_registerInHFWidget->setAnalyteText(s);
}

void AddPeakDialog::onCancelClicked()
{
  reject();
}

void AddPeakDialog::onOkClicked()
{
  for (int idx = 0; idx < m_analytesModel.rowCount(); idx++) {
    const QStandardItem *item = m_analytesModel.item(idx, 0);
    const QString &s = item->text();

    if (s == ui->qcbox_analyte->currentText()) {
      accept();
      return;
    }
  }

  QStandardItem *item = new QStandardItem(ui->qcbox_analyte->currentText());
  m_analytesModel.appendRow(item);

  accept();
}

void AddPeakDialog::onRegisterClicked()
{
  bool checked = ui->qcb_register->checkState() == Qt::Checked;

  m_registerInHFWidget->setVisible(checked);
}

void AddPeakDialog::setInformation(const double selConcentration, const double hvlMobility, const double peakMaxMobility)
{
  m_registerInHFWidget->setInformation(ui->qcbox_analyte->currentText(), selConcentration, hvlMobility, peakMaxMobility);
}
