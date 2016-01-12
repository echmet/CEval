#include "adjustplotvisualsdialog.h"
#include "ui_adjustplotvisualsdialog.h"
#include <QColorDialog>
#include <QStandardItemModel>

AdjustPlotVisualsDialog::AxisVisuals::AxisVisuals(const SerieProperties::Axis a) :
  a(a)
{
}

void AdjustPlotVisualsDialog::AxisVisuals::fromOther(const AxisVisuals &other)
{
  fontSize = other.fontSize;
  bold = other.bold;
}

AdjustPlotVisualsDialog::AxisVisuals &AdjustPlotVisualsDialog::AxisVisuals::operator=(const AxisVisuals &other)
{
  const_cast<SerieProperties::Axis&>(a) = other.a;
  fontSize = other.fontSize;
  bold = other.bold;

  return *this;
}

AdjustPlotVisualsDialog::SerieVisuals::SerieVisuals(const int id) :
  id(id)
{
}

void AdjustPlotVisualsDialog::SerieVisuals::fromOther(const SerieVisuals &other)
{
  lineColor = other.lineColor;
  lineThickness = other.lineThickness;
  pointLineThickness = other.pointLineThickness;
  pointColor = other.pointColor;
  pointFillColor = other.pointFillColor;
  pointSize = other.pointSize;
  pointStyle = other.pointStyle;
}

AdjustPlotVisualsDialog::SerieVisuals &AdjustPlotVisualsDialog::SerieVisuals::operator=(const SerieVisuals &other)
{
  const_cast<int&>(id) = other.id;
  lineColor = other.lineColor;
  lineThickness = other.lineThickness;
  pointLineThickness = other.pointLineThickness;
  pointColor = other.pointColor;
  pointFillColor = other.pointFillColor;
  pointSize = other.pointSize;
  pointStyle = other.pointStyle;

  return *this;
}

AdjustPlotVisualsDialog::AdjustPlotVisualsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AdjustPlotVisualsDialog)
{
  ui->setupUi(this);
  ui->ql_lineColorClr->setAutoFillBackground(true);
  ui->ql_pointColorClr->setAutoFillBackground(true);
  ui->ql_pointFillColorClr->setAutoFillBackground(true);

  fillPointStylesComboBox();

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onOkClicked);

  connect(ui->qpb_pickLineColor, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onPickLineColorClicked);
  connect(ui->qpb_pickPointColor, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onPickPointColorClicked);
  connect(ui->qpb_pickPointFillColor, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onPickPointFillColorClicked);
  connect(ui->qpb_setForAllAxes, &QPushButton::clicked, this, &AdjustPlotVisualsDialog::onSetForAllAxesClicked);

  connect(ui->qspbox_lineThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AdjustPlotVisualsDialog::onLineThicknessChanged);
  connect(ui->qspbox_pointSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &AdjustPlotVisualsDialog::onPointSizeChanged);
  connect(ui->qspbox_pointLineThickness, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AdjustPlotVisualsDialog::onPointLineThicknessChanges);

  connect(ui->qspbox_axisFontSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &AdjustPlotVisualsDialog::onAxisFontSizeChanged);
  connect(ui->qcb_axisBold, &QCheckBox::stateChanged, this, &AdjustPlotVisualsDialog::onAxisFontBoldChanged);

  connect(ui->qcbox_series, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AdjustPlotVisualsDialog::onSerieSelected);
  connect(ui->qcbox_pointStyles, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &AdjustPlotVisualsDialog::onPointStyleSelected);
  connect(ui->qcbox_axis, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AdjustPlotVisualsDialog::onAxisSelected);

}

AdjustPlotVisualsDialog::~AdjustPlotVisualsDialog()
{
  delete ui;
}

void AdjustPlotVisualsDialog::addAxisVisuals(const QString &title, const AxisVisuals &av)
{
  ui->qcbox_axis->addItem(title, QVariant::fromValue<AxisVisuals>(av));
}

void AdjustPlotVisualsDialog::addSerieVisuals(const QString &title, const SerieVisuals &sv)
{
  ui->qcbox_series->addItem(title, QVariant::fromValue<SerieVisuals>(sv));
}

QVector<AdjustPlotVisualsDialog::AxisVisuals> AdjustPlotVisualsDialog::axisVisuals() const
{
  return vectorOfVisuals<AxisVisuals>(ui->qcbox_axis);
}

QString AdjustPlotVisualsDialog::backgroundColorToStyleSheet(const QColor &c) const
{
  return QString(" QLabel { background-color: rgb(%1,%2,%3); }").arg(c.red()).arg(c.green()).arg(c.blue());
}

QStandardItem *AdjustPlotVisualsDialog::comboBoxItem(const QComboBox *qcbox, const int idx) const
{
  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(qcbox->model());
  Q_ASSERT(model != nullptr);

  return model->item(idx);
}

template<typename T>
T AdjustPlotVisualsDialog::datatypeFromItem(const QStandardItem *item) const
{
  QVariant v = item->data(Qt::UserRole);
  Q_ASSERT(v.canConvert<T>());

  return v.value<T>();
}

void AdjustPlotVisualsDialog::fillPointStylesComboBox()
{
  QComboBox *qcbox = ui->qcbox_pointStyles;
  for (int idx = 0; idx < static_cast<int>(PointStyles::LAST_INDEX); idx++) {
    PointStyles ps = static_cast<PointStyles>(idx);
    qcbox->addItem(pointStyleName(ps), QVariant::fromValue<PointStyles>(ps));
  }
}

void AdjustPlotVisualsDialog::onAxisFontBoldChanged(const int c)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_axis, ui->qcbox_axis->currentIndex());
  if (item == nullptr)
    return;

  AxisVisuals av = datatypeFromItem<AxisVisuals>(item);

  av.bold = (c == Qt::Checked);
  item->setData(QVariant::fromValue<AxisVisuals>(av), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onAxisFontSizeChanged(const int s)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_axis, ui->qcbox_axis->currentIndex());
  if (item == nullptr)
    return;

  AxisVisuals av = datatypeFromItem<AxisVisuals>(item);

  av.fontSize = s;
  item->setData(QVariant::fromValue<AxisVisuals>(av), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onAxisSelected(const int idx)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_axis, idx);
  if (item == nullptr)
    return;

  AxisVisuals av = datatypeFromItem<AxisVisuals>(item);

  ui->qspbox_axisFontSize->setValue(av.fontSize);
  ui->qcb_axisBold->setChecked(av.bold);
}

void AdjustPlotVisualsDialog::onCancelClicked()
{
  reject();
}

void AdjustPlotVisualsDialog::onLineThicknessChanged(const double t)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  sv.lineThickness = t;
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onOkClicked()
{
  accept();
}

void AdjustPlotVisualsDialog::onPickLineColorClicked()
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  QColorDialog dlg(sv.lineColor);
  int dlgRet = dlg.exec();
  if (dlgRet != QDialog::Accepted)
    return;

  sv.lineColor = dlg.currentColor();
  setLineColorBox(sv.lineColor);
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onPickPointColorClicked()
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  QColorDialog dlg(sv.pointColor);
  int dlgRet = dlg.exec();

  if (dlgRet != QDialog::Accepted)
    return;

  sv.pointColor = dlg.currentColor();
  setPointColorBox(dlg.currentColor());
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onPickPointFillColorClicked()
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  QColorDialog dlg(sv.pointFillColor);
  int dlgRet = dlg.exec();

  if (dlgRet != QDialog::Accepted)
    return;

  sv.pointFillColor = dlg.currentColor();
  setPointFillColorBox(dlg.currentColor());
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onPointLineThicknessChanges(const double t)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  sv.pointLineThickness = t;
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onPointSizeChanged(const int t)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  sv.pointSize = t;
  item->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onPointStyleSelected(const int idx)
{
  QStandardItem *pointItem = comboBoxItem(ui->qcbox_pointStyles, idx);
  QStandardItem *serieItem = comboBoxItem(ui->qcbox_series, ui->qcbox_series->currentIndex());

  if (pointItem == nullptr || serieItem == nullptr)
    return;

  PointStyles ps = datatypeFromItem<PointStyles>(pointItem);
  SerieVisuals sv = datatypeFromItem<SerieVisuals>(serieItem);

  sv.pointStyle = ps;

  serieItem->setData(QVariant::fromValue<SerieVisuals>(sv), Qt::UserRole);
}

void AdjustPlotVisualsDialog::onSerieSelected(const int idx)
{
  QStandardItem *item = comboBoxItem(ui->qcbox_series, idx);
  if (item == nullptr)
    return;

  SerieVisuals sv = datatypeFromItem<SerieVisuals>(item);

  setLineColorBox(sv.lineColor);
  setPointColorBox(sv.pointColor);
  setPointStyleIndex(sv.pointStyle);
  setPointFillColorBox(sv.pointFillColor);

  ui->qspbox_lineThickness->setValue(sv.lineThickness);
  ui->qspbox_pointLineThickness->setValue(sv.pointLineThickness);
  ui->qspbox_pointSize->setValue(sv.pointSize);
}

void AdjustPlotVisualsDialog::onSetForAllAxesClicked()
{
  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->qcbox_axis->model());
  Q_ASSERT(model != nullptr);

  QStandardItem *item = comboBoxItem(ui->qcbox_axis, ui->qcbox_axis->currentIndex());
  if (item == nullptr)
    return;

  const AxisVisuals targetAv = datatypeFromItem<AxisVisuals>(item);

  for (int idx = 0; idx < model->rowCount(); idx++) {
    item = model->item(idx);
    if (item == nullptr)
      continue;

    AxisVisuals av = datatypeFromItem<AxisVisuals>(item);
    av.fromOther(targetAv);

    item->setData(QVariant::fromValue<AxisVisuals>(av), Qt::UserRole);
  }
}

QString AdjustPlotVisualsDialog::pointStyleName(const PointStyles ps)
{
  switch (ps) {
  case PointStyles::CROSS:
    return tr("Cross");
    break;
  case PointStyles::DIAMOND:
    return tr("Diamond");
    break;
  case PointStyles::D_TRIANGLE:
    return tr("Downward triangle");
    break;
  case PointStyles::ELLIPSE:
    return tr("Ellipse");
    break;
  case PointStyles::HEXAGON:
    return tr("Hexagon"); /* I am now a hexagon in two-dimensional space... */
    break;
  case PointStyles::H_LINE:
    return tr("Horizontal line");
    break;
  case PointStyles::L_TRIANGLE:
    return tr("Leftward triangle");
    break;
  case PointStyles::NO_SYMBOL:
    return tr("No symbol");
    break;
  case PointStyles::RECT:
    return tr("Rectangle");
    break;
  case PointStyles::R_TRIANGLE:
    return tr("Right triangle");
    break;
  case PointStyles::STAR_A:
    return tr("Star 1");
    break;
  case PointStyles::STAR_B:
    return tr("Star 2");
    break;
  case PointStyles::TRIANGLE:
    return tr("Triangle");
    break;
  case PointStyles::U_TRIANGLE:
    return tr("Upward triangle");
    break;
  case PointStyles::V_LINE:
    return tr("Vertical line");
    break;
  case PointStyles::X_CROSS:
    return tr("X cross");
    break;
  default:
    return "";
  }
}

QVector<AdjustPlotVisualsDialog::SerieVisuals> AdjustPlotVisualsDialog::serieVisuals() const
{
  return vectorOfVisuals<SerieVisuals>(ui->qcbox_series);
}

void AdjustPlotVisualsDialog::setLineColorBox(const QColor &c)
{
    ui->ql_lineColorClr->setStyleSheet(backgroundColorToStyleSheet(c));
}

void AdjustPlotVisualsDialog::setPointColorBox(const QColor &c)
{
  ui->ql_pointColorClr->setStyleSheet(backgroundColorToStyleSheet(c));
}

void AdjustPlotVisualsDialog::setPointFillColorBox(const QColor &c)
{
  ui->ql_pointFillColorClr->setStyleSheet(backgroundColorToStyleSheet(c));
}

void AdjustPlotVisualsDialog::setPointStyleIndex(const PointStyles ps)
{
  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->qcbox_pointStyles->model());
  if (model == nullptr)
    return;

  for (int idx = 0; idx < model->rowCount(); idx++) {
    QStandardItem *item = model->item(idx);
    PointStyles ips = datatypeFromItem<PointStyles>(item);

    if (ips == ps) {
      ui->qcbox_pointStyles->setCurrentIndex(idx);
      return;
    }
  }
}

template<typename T>
QVector<T> AdjustPlotVisualsDialog::vectorOfVisuals(const QComboBox *qcbox) const
{
  QVector<T> vec;

  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(qcbox->model());
  Q_ASSERT(model != nullptr);

  for (int idx = 0; idx < model->rowCount(); idx++) {
    QVariant v = model->data(model->index(idx, 0), Qt::UserRole);
    if (!v.isValid())
      continue;

    if (!v.canConvert<T>())
      continue;

    T vis = v.value<T>();
    vec.push_back(vis);
  }

  return vec;
}
