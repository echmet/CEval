#include "setaxistitlesdialog.h"
#include "ui_setaxistitlesdialog.h"

SetAxisTitlesDialog::SetAxisTitlesDialog(const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SetAxisTitlesDialog)
{
  ui->setupUi(this);

  ui->qle_xType->setText(xType);
  ui->qle_xUnit->setText(xUnit);
  ui->qle_yType->setText(yType);
  ui->qle_yUnit->setText(yUnit);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &SetAxisTitlesDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &SetAxisTitlesDialog::onOkClicked);
}

SetAxisTitlesDialog::~SetAxisTitlesDialog()
{
  delete ui;
}

void SetAxisTitlesDialog::onCancelClicked()
{
  reject();
}

void SetAxisTitlesDialog::onOkClicked()
{
  accept();
}

QString SetAxisTitlesDialog::xType() const
{
  return ui->qle_xType->text();
}

QString SetAxisTitlesDialog::xUnit() const
{
  return ui->qle_xUnit->text();
}

QString SetAxisTitlesDialog::yType() const
{
  return ui->qle_yType->text();
}

QString SetAxisTitlesDialog::yUnit() const
{
  return ui->qle_yUnit->text();
}

