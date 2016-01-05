#include "addpeakdialog.h"
#include "ui_addpeakdialog.h"

AddPeakDialog::Answer::Answer(const bool registerInHF, const QString &name) :
  registerInHF(registerInHF),
  name(name)
{
}

AddPeakDialog::AddPeakDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddPeakDialog)
{
  ui->setupUi(this);

  ui->qcbox_analyte->setModel(&m_analytesModel);

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
                ui->qcbox_analyte->currentText());
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

void AddPeakDialog::onOkClicked()
{
  QStandardItem *item = new QStandardItem(ui->qcbox_analyte->currentText());
  m_analytesModel.appendRow(item);

  accept();
}

void AddPeakDialog::onRegisterClicked()
{
  bool checked = ui->qcb_register->checkState() == Qt::Checked;

  ui->ql_hfAnalyte->setVisible(checked);
  ui->ql_hfAnalyteRes->setVisible(checked);
  ui->ql_hfMobility->setVisible(checked);
  ui->ql_hfMobilityRes->setVisible(checked);
  ui->ql_hfSelConc->setVisible(checked);
  ui->ql_hfSelConcRes->setVisible(checked);
}

void AddPeakDialog::setInformation(const double selConcentration, const double mobility)
{
  ui->ql_hfSelConcRes->setText(QString::number(selConcentration));
  ui->ql_hfMobilityRes->setText(QString::number(mobility));

  if (!std::isfinite(mobility)) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, Qt::red);

    ui->ql_hfMobility->setPalette(palette);
    ui->ql_hfMobilityRes->setPalette(palette);
  } else {
    ui->ql_hfMobility->setPalette(QPalette());
    ui->ql_hfMobilityRes->setPalette(QPalette());
  }
}
