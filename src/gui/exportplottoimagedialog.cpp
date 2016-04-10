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
  connect(ui->qle_path, &QLineEdit::textChanged, this, &ExportPlotToImageDialog::onFilePathChanged);
  connect(ui->qspbox_height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onHeightChanged);
  connect(ui->qspbox_width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onWidthChanged);
  connect(ui->qcb_keepRatio, &QCheckBox::clicked, this, &ExportPlotToImageDialog::onKeepAspectRatioClicked);
}

ExportPlotToImageDialog::~ExportPlotToImageDialog()
{
  delete ui;
}

void ExportPlotToImageDialog::onBrowseClicked()
{
  QFileDialog dlg;

  dlg.setAcceptMode(QFileDialog::AcceptSave);

  if (dlg.exec() != QDialog::Accepted)
    return;

  ui->qle_path->setText(dlg.selectedFiles()[0]);
}

void ExportPlotToImageDialog::onCancelClicked()
{
  reject();
}

void ExportPlotToImageDialog::onFilePathChanged(const QString &path)
{
  int idx = path.lastIndexOf(".");

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
}

void ExportPlotToImageDialog::onHeightChanged(const double h)
{
  if (ui->qcb_keepRatio->checkState() == Qt::Checked) {
    disconnect(ui->qspbox_width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onWidthChanged);
    ui->qspbox_width->setValue(m_aspectRatio * h);
    connect(ui->qspbox_width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onWidthChanged);
  }
}

void ExportPlotToImageDialog::onKeepAspectRatioClicked()
{
  if (ui->qcb_keepRatio->checkState() == Qt::Checked) {
    const double w = ui->qspbox_width->value();
    const double h = ui->qspbox_height->value();

    m_aspectRatio = w / h;
  }
}

void ExportPlotToImageDialog::onOkClicked()
{
  accept();
}

void ExportPlotToImageDialog::onWidthChanged(const double w)
{
  if (ui->qcb_keepRatio->checkState() == Qt::Checked) {
    disconnect(ui->qspbox_height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onHeightChanged);
    ui->qspbox_height->setValue(w / m_aspectRatio);
    connect(ui->qspbox_height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onHeightChanged);
  }
}

ExportPlotToImageDialog::Parameters ExportPlotToImageDialog::parameters() const
{
  return Parameters(ui->qle_path->text(), ui->qcbox_format->currentData().toString(),
                    QSizeF(ui->qspbox_width->value(), ui->qspbox_height->value()),
                    ui->qspbox_dpi->value());
}

void ExportPlotToImageDialog::setPlotDimensions(const QSizeF &dimensions)
{
  disconnect(ui->qspbox_height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onHeightChanged);
  disconnect(ui->qspbox_width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onWidthChanged);
  ui->qspbox_width->setValue(dimensions.width());
  ui->qspbox_height->setValue(dimensions.height());
  connect(ui->qspbox_height, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onHeightChanged);
  connect(ui->qspbox_width, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ExportPlotToImageDialog::onWidthChanged);

  m_aspectRatio = dimensions.width() / dimensions.height();
}
