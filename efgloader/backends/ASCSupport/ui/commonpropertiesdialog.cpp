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

SupportedEncodings::EncodingType CommonPropertiesDialog::encoding() const
{
  const QVariant v = ui->qcbox_encoding->currentData();
  if (!v.canConvert<SupportedEncodings::EncodingType>())
    return SupportedEncodings::INVALID_ENCTYPE;

  return v.value<SupportedEncodings::EncodingType>();
}

void CommonPropertiesDialog::fillEncodings(const SupportedEncodings::EncodingsVec &encs)
{
  for (const auto &e : encs)
    ui->qcbox_encoding->addItem(QString::fromStdString(e.first), QVariant::fromValue<SupportedEncodings::EncodingType>(e.second));
}
