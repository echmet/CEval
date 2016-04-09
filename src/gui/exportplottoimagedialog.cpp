#include "exportplottoimagedialog.h"
#include "ui_exportplottoimagedialog.h"
#include <QFileDialog>

ExportPlotToImageDialog::Parameters::Parameters(const QString &path, const QString &format, const QSizeF &dimensions, const int dpi) :
  path(path),
  format(format),
  dimensions(dimensions),
  dpi(dpi)
{
}

ExportPlotToImageDialog::Parameters &ExportPlotToImageDialog::Parameters::operator=(const Parameters &other)
{
  const_cast<QString&>(path) = other.path;
  const_cast<QString&>(format) = other.format;
  const_cast<QSizeF&>(dimensions) = other.dimensions;
  const_cast<int&>(dpi) = other.dpi;

  return *this;
}

ExportPlotToImageDialog::ExportPlotToImageDialog(const QStringList &supportedFormats, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ExportPlotToImageDialog)
{
  ui->setupUi(this);

  for (const QString &s : supportedFormats)
    ui->qcbox_format->addItem(s, s);

  connect(ui->qpb_browse, &QPushButton::clicked, this, &ExportPlotToImageDialog::onBrowseClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &ExportPlotToImageDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &ExportPlotToImageDialog::onOkClicked);
}

ExportPlotToImageDialog::~ExportPlotToImageDialog()
{
  delete ui;
}

void ExportPlotToImageDialog::onBrowseClicked()
{
  QFileDialog dlg;
  QString path;
  int idx;

  dlg.setAcceptMode(QFileDialog::AcceptSave);

  if (dlg.exec() != QDialog::Accepted)
    return;

  path = dlg.selectedFiles()[0];
  idx = path.lastIndexOf(".");
  if (idx > -1) {
    QString suffix;

    suffix = path.mid(idx + 1);
    for (int jdx = 0; jdx < ui->qcbox_format->count(); jdx++) {
      QString formatTag = ui->qcbox_format->itemData(jdx).toString();

      if (suffix == formatTag) {
        ui->qcbox_format->setCurrentIndex(jdx);
        break;
      }
    }
  }

  ui->qle_path->setText(dlg.selectedFiles()[0]);
}

void ExportPlotToImageDialog::onCancelClicked()
{
  reject();
}

void ExportPlotToImageDialog::onOkClicked()
{
  accept();
}

ExportPlotToImageDialog::Parameters ExportPlotToImageDialog::parameters() const
{
  return Parameters(ui->qle_path->text(), ui->qcbox_format->currentData().toString(),
                    QSizeF(ui->qspbox_width->value(), ui->qspbox_height->value()),
                    ui->qspbox_dpi->value());
}

void ExportPlotToImageDialog::setPlotDimensions(const QSizeF &dimensions)
{
  ui->qspbox_width->setValue(dimensions.width());
  ui->qspbox_height->setValue(dimensions.height());
}
