#include "numberformatdialog.h"
#include "ui_numberformatdialog.h"
#include "../doubletostringconvertor.h"

NumberFormatDialog::NumberFormatDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NumberFormatDialog)
{
  ui->setupUi(this);

  m_intValidator.setBottom(1);
  m_intValidator.setTop(16);

  /* Fill list of available locales */
  {
    QString currentName = DoubleToStringConvertor::locale().name();
    int locIdx = 0;
    int ctr = 0;
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::Language::AnyLanguage, QLocale::Script::AnyScript, QLocale::Country::AnyCountry);
    for (const QLocale &loc : allLocales) {
      ui->qcbox_formatting->addItem(QString("%1 (%2)").arg(loc.nativeCountryName()).arg(loc.bcp47Name()), loc.name());
      if (loc.name() == currentName)
        locIdx = ctr;
      ctr++;
    }
    ui->qcbox_formatting->setCurrentIndex(locIdx);
  }

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &NumberFormatDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &NumberFormatDialog::onOkClicked);
}

NumberFormatDialog::~NumberFormatDialog()
{
  delete ui;
}

void NumberFormatDialog::onCancelClicked()
{
  reject();
}

void NumberFormatDialog::onOkClicked()
{
  char type;
  int digits;
  QString locName;
  bool ok;

  if (ui->qcb_trailingZeros->checkState() == Qt::Checked)
    type = 'f';
  else
    type = 'g';

  digits = ui->qle_digits->text().toInt(&ok);
  if (!ok)
    digits = 5;

  locName = ui->qcbox_formatting->currentData().toString();

  DoubleToStringConvertor::setParameters(type, digits, locName);

  accept();
}

void NumberFormatDialog::setParameters(const char type, int digits)
{
  if (type == 'f')
    ui->qcb_trailingZeros->setCheckState(Qt::Checked);
  else
    ui->qcb_trailingZeros->setCheckState(Qt::Unchecked);

  if (digits > 0 && digits < 17)
    ui->qle_digits->setText(QString::number(digits));
  else
    ui->qle_digits->setText("5");
}
