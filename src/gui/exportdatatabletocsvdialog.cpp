#include "exportdatatabletocsvdialog.h"
#include "ui_exportdatatabletocsvdialog.h"
#include <QFileDialog>

ExportDatatableToCsvDialog::Parameters::Parameters() :
  exMode(ExportMode::SINGLE_FILE),
  path(""),
  decimalSeparator(QChar('\0')),
  delimiter(""),
  precision(0)
{
}

ExportDatatableToCsvDialog::Parameters::Parameters(const Parameters &other) :
  exMode(other.exMode),
  path(other.path),
  decimalSeparator(other.decimalSeparator),
  delimiter(other.delimiter),
  precision(other.precision)
{
}

ExportDatatableToCsvDialog::Parameters::Parameters(const ExportMode exMode, const QString &path, const QChar &decimalSeparator,
                                                   const QString &delimiter, const int precision) :
  exMode(exMode),
  path(path),
  decimalSeparator(decimalSeparator),
  delimiter(delimiter),
  precision(precision)
{
}

ExportDatatableToCsvDialog::ExportDatatableToCsvDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ExportDatatableToCsvDialog),
  m_lastPath(QDir::homePath())
{
  ui->setupUi(this);

  ui->qcbox_mode->addItem(QObject::tr("All analytes in a single file"), QVariant::fromValue<ExportMode>(ExportMode::SINGLE_FILE));

  ui->qcbox_decimalSeparator->addItem(QObject::tr("Comma (,)"), QChar(','));
  ui->qcbox_decimalSeparator->addItem(QObject::tr("Period (.)"), QChar('.'));

  connect(ui->qpb_browse, &QPushButton::clicked, this, &ExportDatatableToCsvDialog::onBrowseClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &ExportDatatableToCsvDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &ExportDatatableToCsvDialog::onOkClicked);
}

ExportDatatableToCsvDialog::~ExportDatatableToCsvDialog()
{
  delete ui;
}

ExportDatatableToCsvDialog::Parameters ExportDatatableToCsvDialog::parameters() const
{
  ExportMode exMode;

  if (!ui->qcbox_mode->currentData().canConvert<ExportMode>())
    return Parameters();
  exMode = ui->qcbox_mode->currentData().value<ExportMode>();

  return Parameters(exMode, ui->qle_path->text(), ui->qcbox_decimalSeparator->currentData().toChar(),
                    ui->qle_delimiter->text(), ui->qspbox_numericPrecision->value());
}

void ExportDatatableToCsvDialog::onBrowseClicked()
{
  QFileDialog dlg;
  ExportMode exMode;

  if (!ui->qcbox_mode->currentData().canConvert<ExportMode>())
    return;
  exMode = ui->qcbox_mode->currentData().value<ExportMode>();

  dlg.setAcceptMode(QFileDialog::AcceptSave);

  switch (exMode) {
  case ExportMode::SINGLE_FILE:
    dlg.setFileMode(QFileDialog::AnyFile);
    break;
  }

  if (dlg.exec() != QDialog::Accepted)
    return;

  ui->qle_path->setText(dlg.selectedFiles()[0]);
}

void ExportDatatableToCsvDialog::onCancelClicked()
{
  reject();
}

void ExportDatatableToCsvDialog::onOkClicked()
{
  accept();
}
