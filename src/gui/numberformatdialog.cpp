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
    QLocale::Country currentCtry = DoubleToStringConvertor::country();
    int currentCtryIdx = 0;
    for (int ctry = 1; ctry <= 246; ctry++) {
      QLocale::Country tctry = static_cast<QLocale::Country>(ctry);

      ui->qcbox_formatting->addItem(QLocale::countryToString(tctry), ctry);
      if (tctry == currentCtry)
        currentCtryIdx = ctry - 1;
    }
    ui->qcbox_formatting->setCurrentIndex(currentCtryIdx);
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
  int ctry;
  bool ok;

  if (ui->qcb_trailingZeros->checkState() == Qt::Checked)
    type = 'f';
  else
    type = 'g';

  digits = ui->qle_digits->text().toInt(&ok);
  if (!ok)
    digits = 5;

  ctry = ui->qcbox_formatting->currentData().toInt();

  DoubleToStringConvertor::setParameters(type, digits, static_cast<QLocale::Country>(ctry));

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
