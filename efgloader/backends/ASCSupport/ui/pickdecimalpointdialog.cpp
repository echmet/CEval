#include "pickdecimalpointdialog.h"
#include "ui_pickdecimalpointdialog.h"

PickDecimalPointDialog::PickDecimalPointDialog(const QString &name, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::PickDecimalPointDialog}
{
  QString caption = QString{"ASC parser cannot deduce the decimal separator in file\n\n%1\n\nPlease specify it manually."}.arg(name);

  ui->setupUi(this);
  ui->ql_caption->setText(caption);

  ui->qcbox_separator->addItem("Period (.)", '.');
  ui->qcbox_separator->addItem("Comma (,)", ',');

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

PickDecimalPointDialog::~PickDecimalPointDialog()
{
  delete ui;
}

char PickDecimalPointDialog::separator() const
{
  const QVariant v = ui->qcbox_separator->currentData();
  if (!v.canConvert<QChar>())
    return '.';

  return v.toChar().toLatin1();
}
