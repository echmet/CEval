#include "commonpropertiesdialog.h"
#include "ui_commonpropertiesdialog.h"

CommonPropertiesDialog::CommonPropertiesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CommonPropertiesDialog)
{
  ui->setupUi(this);
}

CommonPropertiesDialog::~CommonPropertiesDialog()
{
  delete ui;
}
