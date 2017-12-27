#include "commonpropertiesdialog.h"
#include "ui_commonpropertiesdialog.h"

CommonPropertiesDialog::CommonPropertiesDialog(const SupportedEncodings::EncodingsVec &encs, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::CommonPropertiesDialog}
{
  ui->setupUi(this);

  fillEncodings(encs);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

CommonPropertiesDialog::~CommonPropertiesDialog()
{
  delete ui;
}

std::string CommonPropertiesDialog::encoding() const
{
  const QVariant v = ui->qcbox_encoding->currentData();
  if (!v.canConvert<QString>())
    return "";

  return v.toString().toStdString();
}

void CommonPropertiesDialog::fillEncodings(const SupportedEncodings::EncodingsVec &encs)
{
  for (const auto &e : encs)
    ui->qcbox_encoding->addItem(QString::fromStdString(e.first), QString::fromStdString(e.second));
}
