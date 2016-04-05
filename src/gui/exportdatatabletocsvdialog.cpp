#include "exportdatatabletocsvdialog.h"
#include "ui_exportdatatabletocsvdialog.h"

ExportDatatableToCsvDialog::ExportDatatableToCsvDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ExportDatatableToCsvDialog)
{
  ui->setupUi(this);
}

ExportDatatableToCsvDialog::~ExportDatatableToCsvDialog()
{
  delete ui;
}
