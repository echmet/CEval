#include "textexporterbackendconfigurationdialog.h"
#include "ui_textexporterbackendconfigurationdialog.h"

TextExporterBackendConfigurationDialog::TextExporterBackendConfigurationDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::TextExporterBackendConfigurationDialog)
{
  ui->setupUi(this);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &TextExporterBackendConfigurationDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &TextExporterBackendConfigurationDialog::onOkClicked);
}

TextExporterBackendConfigurationDialog::~TextExporterBackendConfigurationDialog()
{
  delete ui;
}

QString TextExporterBackendConfigurationDialog::interact(bool &canceled, const QChar &currentDelimiter)
{
  if (currentDelimiter == QChar('\t'))
    ui->qle_delimiter->setText("\\t");
  else
    ui->qle_delimiter->setText(currentDelimiter);

  if (this->exec() != QDialog::Accepted) {
    canceled = true;
    return "";
  }

  canceled = false;
  return ui->qle_delimiter->text();
}

void TextExporterBackendConfigurationDialog::onCancelClicked()
{
  reject();
}

void TextExporterBackendConfigurationDialog::onOkClicked()
{
  accept();
}
