#include "checkforupdatedialog.h"
#include "ui_checkforupdatedialog.h"

CheckForUpdateDialog::CheckForUpdateDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CheckForUpdateDialog)
{
  ui->setupUi(this);
}

CheckForUpdateDialog::~CheckForUpdateDialog()
{
  delete ui;
}
