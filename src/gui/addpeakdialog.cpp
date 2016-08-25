#include "addpeakdialog.h"
#include "ui_addpeakdialog.h"
#include "../doubletostringconvertor.h"
#include <cmath>

AddPeakDialog::Answer::Answer(const bool registerInHF, const QString &name, const MobilityFrom mobilityFrom) :
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

  ui->qcbox_analyte->setModel(&m_analytesModel);

  ui->qcbox_mobilityFrom->addItem(tr("HVL a1 parameter"), QVariant::fromValue<MobilityFrom>(MobilityFrom::HVL_A1));
  ui->qcbox_mobilityFrom->addItem(tr("Peak maximum"), QVariant::fromValue<MobilityFrom>(MobilityFrom::PEAK_MAXIMUM));


  connect(ui->qpb_cancel, &QPushButton::clicked, this, &AddPeakDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &AddPeakDialog::onOkClicked);
  connect(ui->qcb_register, &QCheckBox::clicked, this, &AddPeakDialog::onRegisterClicked);
  connect(ui->qcbox_analyte, &QComboBox::currentTextChanged, this, &AddPeakDialog::onAnalyteTextChanged);
  connect(ui->qcbox_mobilityFrom, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &AddPeakDialog::onMobilityFromActivated);

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
                ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>()
                );
}

void AddPeakDialog::onAnalyteTextChanged(const QString &s)
{
  ui->ql_hfAnalyteRes->setText(s);

  if (s.length() < 1) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->ql_hfAnalyte->setPalette(palette);
  } else {
    ui->ql_hfAnalyte->setPalette(QPalette());
  }
}

void AddPeakDialog::onCancelClicked()
{
  reject();
}

void AddPeakDialog::onMobilityFromActivated(const int idx)
{
  Q_UNUSED(idx);

  setMobilityText(ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>());
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

  ui->ql_mobilityFrom->setVisible(checked);
  ui->qcbox_mobilityFrom->setVisible(checked);
  ui->ql_hfAnalyte->setVisible(checked);
  ui->ql_hfAnalyteRes->setVisible(checked);
  ui->ql_hfMobility->setVisible(checked);
  ui->ql_hfMobilityRes->setVisible(checked);
  ui->ql_hfSelConc->setVisible(checked);
  ui->ql_hfSelConcRes->setVisible(checked);
}

void AddPeakDialog::setInformation(const double selConcentration, const double hvlMobility, const double peakMaxMobility)
{
  ui->ql_hfSelConcRes->setText(QString::number(selConcentration));

  m_hvlMobility = hvlMobility;
  m_peakMaxMobility = peakMaxMobility;

  setMobilityText(ui->qcbox_mobilityFrom->currentData().value<MobilityFrom>());
}

void AddPeakDialog::setMobilityText(const MobilityFrom m)
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

    ui->ql_hfMobility->setPalette(palette);
    ui->ql_hfMobilityRes->setPalette(palette);
    ui->ql_hfMobilityRes->setText("Invalid");
  } else {
    ui->ql_hfMobility->setPalette(QPalette());
    ui->ql_hfMobilityRes->setPalette(QPalette());

    ui->ql_hfMobilityRes->setText(DoubleToStringConvertor::convert(dm));
  }
}
