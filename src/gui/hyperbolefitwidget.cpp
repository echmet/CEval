#include "hyperbolefitwidget.h"
#include "ui_hyperbolefitwidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QStandardItem>
#include <limits>
#include "../hyperbolefittingwidgetconnector.h"
#include "../doubletostringconvertor.h"

HyperboleFitWidget::HyperboleFitWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::HyperboleFitWidget)
{
  ui->setupUi(this);

  connect(ui->qpb_addAnalyte, &QPushButton::clicked, this, &HyperboleFitWidget::onAddAnalyteClicked);
  connect(ui->qpb_removeAnalyte, &QPushButton::clicked, this, &HyperboleFitWidget::onRemoveAnalyteClicked);
  connect(ui->qlv_analytes, &QListView::doubleClicked, this, &HyperboleFitWidget::onAnalyteListDoubleClicked);

  connect(ui->qpb_addConcentration, &QPushButton::clicked, this, &HyperboleFitWidget::onAddConcentrationClicked);
  connect(ui->qpb_removeConcentration, &QPushButton::clicked, this, &HyperboleFitWidget::onRemoveConcentrationClicked);

  connect(ui->qpb_addMobility, &QPushButton::clicked, this, &HyperboleFitWidget::onAddMobilityClicked);
  connect(ui->qpb_removeMobility, &QPushButton::clicked, this, &HyperboleFitWidget::onRemoveMobilityClicked);

  connect(ui->qcbox_fitMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperboleFitWidget::onFitModeActivated);
  connect(ui->qcbox_statData, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperboleFitWidget::onStatModeActivated);
  connect(ui->qcbox_statUnits, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperboleFitWidget::onStatUnitsActivated);
  connect(ui->qrb_leftAnalyte, &QRadioButton::clicked, this, &HyperboleFitWidget::onDisplayedAnalyteChanged);
  connect(ui->qrb_rightAnalyte, &QRadioButton::clicked, this, &HyperboleFitWidget::onDisplayedAnalyteChanged);

  connect(ui->qpb_bothConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatBothClicked);
  connect(ui->qpb_leftConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatLeftClicked);
  connect(ui->qpb_rightConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatRightClicked);
  connect(ui->qcb_chartHorizontalMarker, &QCheckBox::clicked, this, &HyperboleFitWidget::onShowChartHorizontalMarker);
  connect(ui->qcb_chartVerticalAMarker, &QCheckBox::clicked, this, &HyperboleFitWidget::onShowChartVerticalAMarker);
  connect(ui->qcb_chartVerticalBMarker, &QCheckBox::clicked, this, &HyperboleFitWidget::onShowChartVerticalBMarker);
  connect(ui->qle_chartHorizontalMarker, &QLineEdit::textChanged, this, &HyperboleFitWidget::onChartHorizontalMarkerValueChanged);
  connect(ui->qle_chartVerticalAMarker, &QLineEdit::textChanged, this, &HyperboleFitWidget::onChartVerticalAMarkerValueChanged);
  connect(ui->qle_chartVerticalBMarker, &QLineEdit::textChanged, this, &HyperboleFitWidget::onChartVerticalBMarkerValueChanged);
  connect(ui->qpb_chartHorizontalMarkerLeftItsc, &QPushButton::clicked, this, &HyperboleFitWidget::onChartHorizontalMarkerLeftItscClicked);
  connect(ui->qpb_chartHorizontalMarkerRightItsc, &QPushButton::clicked, this, &HyperboleFitWidget::onChartHorizontalMarkerRightItscClicked);
  connect(ui->qpb_chartVerticalAMarkerItsc,&QPushButton::clicked, this, &HyperboleFitWidget::onChartVerticalAMarkerItscClicked);
  connect(ui->qpb_chartVerticalBMarkerItsc,&QPushButton::clicked, this, &HyperboleFitWidget::onChartVerticalBMarkerItscClicked);

  connect(ui->qpb_estimate, &QPushButton::clicked, this, &HyperboleFitWidget::onEstimateClicked);
  connect(ui->qpb_fit, &QPushButton::clicked, this, &HyperboleFitWidget::onFitClicked);
  connect(ui->qpb_redrawPoints, &QPushButton::clicked, this, &HyperboleFitWidget::onRedrawClicked);

  ui->qle_analyteAName->setDisabled(true);
  ui->qle_analyteBName->setDisabled(true);

  /* Make sure that the fitting and stats pane
   * scrollarea is always wide enough for all controls
   * to be dislayed without the need to scroll horizontally
   */
  {
    QScrollArea *sa = ui->scrollArea;
    std::function<void (QObject *, QEvent *)> f = [sa](QObject *object, QEvent *event) {
      Q_UNUSED(object);

      QWidget *w = sa->widget();
      if (w != nullptr && event->type() != QEvent::Resize)
        sa->setMinimumWidth(w->width() + sa->verticalScrollBar()->width());
    };

    m_saEvFilter = new ScrollAreaEventFilter(f, ui->scrollAreaWidgetContents, this);
  }

  DoubleToStringConvertor::notifyOnFormatChanged(this);
}

HyperboleFitWidget::~HyperboleFitWidget()
{
  delete ui;
}

void HyperboleFitWidget::connectToAccumulator(QObject *dac)
{
  HyperboleFittingWidgetConnector::connectAll(this, dac);

  connect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperboleFitWidget::onAnalyteListClicked);
  connect(ui->qlv_concentrations->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &HyperboleFitWidget::onConcentrationsListClicked);
}

void HyperboleFitWidget::onAddAnalyteClicked()
{
  QInputDialog dlg(this);
  int ret;

  dlg.setLabelText(tr("Input name of the analyte"));

  while (true) {
    ret = dlg.exec();

    if (ret == QDialog::Accepted) {
      QString name = dlg.textValue();
      if (name.length() > 0) {
        emit addAnalyte(name);
        m_analytesSortProxy.sort(0);
        return;
      }
    } else
      return;
  }
}

void HyperboleFitWidget::onAddConcentrationClicked()
{
  QInputDialog dlg(this);

  dlg.setLabelText(tr("Enter concentration"));
  dlg.setInputMode(QInputDialog::TextInput);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double d = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }
    emit addConcentration(d);
    m_concentrationsSortProxy.sort(0);
    break;
  }

  return;
}

void HyperboleFitWidget::onAddMobilityClicked()
{
  QInputDialog dlg(this);

  dlg.setLabelText(tr("Enter migration time"));
  dlg.setInputMode(QInputDialog::TextInput);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double d = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }
    emit addMobility(d);
    m_mobilitiesSortProxy.sort(0);
    break;
  }

  return;
}

void HyperboleFitWidget::onAnalyteListClicked()
{
  QModelIndexList list = ui->qlv_analytes->selectionModel()->selectedIndexes();
  QModelIndexList list2;

  /* :-P */
  disconnect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperboleFitWidget::onAnalyteListClicked);
  if (list.size() > 0) {
    ui->qlv_analytes->clearSelection();
    for (int idx = 0; idx < (list.size() > 2 ? 2 : list.size()); idx++) {
      ui->qlv_analytes->setCurrentIndex(list.at(idx));
      list2.push_back(m_analytesSortProxy.mapToSource(list.at(idx)));
    }

    emit analyteSwitched(list2);
  }
  connect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperboleFitWidget::onAnalyteListClicked);
}

void HyperboleFitWidget::onAnalyteListDoubleClicked(const QModelIndex &idx)
{
  QInputDialog dlg(this);
  QVariant internalId;
  QString oldName;
  int ret;

  if (!idx.isValid())
    return;

  internalId = ui->qlv_analytes->model()->data(idx, Qt::UserRole + 1);
  oldName = ui->qlv_analytes->model()->data(idx, Qt::DisplayRole).toString();

  dlg.setLabelText(tr("Input a new name of the analyte"));
  dlg.setTextValue(oldName);

  while (true) {
    ret = dlg.exec();

    if (ret == QDialog::Accepted) {
      QString newName = dlg.textValue();
      if (newName.length() > 0) {
        emit renameAnalyte(internalId, newName, idx.row());
        return;
      }
    } else
      return;
  }
}

void HyperboleFitWidget::onChartHorizontalMarkerLeftItscClicked()
{
  emit chartHorizontalMarkerIntersection(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
}

void HyperboleFitWidget::onChartHorizontalMarkerRightItscClicked()
{
  emit chartHorizontalMarkerIntersection(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
}

void HyperboleFitWidget::onChartVerticalAMarkerItscClicked()
{
  emit chartVerticalMarkerIntersection(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
}

void HyperboleFitWidget::onChartVerticalBMarkerItscClicked()
{
  emit chartVerticalMarkerIntersection(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
}

void HyperboleFitWidget::onChartHorizontalMarkerIntersectionSet(const double d)
{
  ui->qle_chartHorizontalMarker->setText(DoubleToStringConvertor::convert(d));
}

void HyperboleFitWidget::onChartVerticalMarkerIntersectionSet(const HyperboleFittingEngineMsgs::MarkerType marker, const double d)
{
  switch (marker) {
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    ui->qle_chartVerticalAMarker->setText(DoubleToStringConvertor::convert(d));
    break;
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    ui->qle_chartVerticalBMarker->setText(DoubleToStringConvertor::convert(d));
    break;
  default:
    break;
  }
}

void HyperboleFitWidget::onChartHorizontalMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER, d);
}

void HyperboleFitWidget::onChartVerticalAMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, d);
}

void HyperboleFitWidget::onChartVerticalBMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, d);
}

void HyperboleFitWidget::onConcentrationsListClicked(const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  const QModelIndex srcidx = m_concentrationsSortProxy.mapToSource(idx);
  emit concentrationSwitched(srcidx);
}

void HyperboleFitWidget::onDisplayedAnalyteChanged()
{
  if (ui->qrb_leftAnalyte->isChecked())
    emit displayedAnalyteChanged(HyperboleFittingEngineMsgs::AnalyteId::ANALYTE_A);
  if (ui->qrb_rightAnalyte->isChecked())
    emit displayedAnalyteChanged(HyperboleFittingEngineMsgs::AnalyteId::ANALYTE_B);
}

void HyperboleFitWidget::onEnableDoubleFit(const bool enable)
{
  if (enable)
    ui->qlv_analytes->setSelectionMode(QAbstractItemView::MultiSelection);
  else {
    QModelIndexList list = ui->qlv_analytes->selectionModel()->selectedIndexes();

    ui->qlv_analytes->setSelectionMode(QAbstractItemView::SingleSelection);
    if (list.size() > 0)
      ui->qlv_analytes->setCurrentIndex(list.at(0));
  }

  ui->qrb_leftAnalyte->setEnabled(enable);
  ui->qrb_rightAnalyte->setEnabled(enable);
  if (enable)
    onDisplayedAnalyteChanged();
}

void HyperboleFitWidget::onEstimateClicked()
{
  emit doEstimate();
}

void HyperboleFitWidget::onFitClicked()
{
  emit doFit();
}

void HyperboleFitWidget::onFitModeActivated(const int idx)
{
  Q_UNUSED(idx);

  emit fitModeChanged(ui->qcbox_fitMode->currentData(Qt::UserRole + 1));
}

void HyperboleFitWidget::onNumberFormatChanged(const QLocale *oldLocale)
{
  Q_UNUSED(oldLocale);

  ui->qlv_analytes->setLocale(DoubleToStringConvertor::locale());
  ui->qlv_concentrations->setLocale(DoubleToStringConvertor::locale());
  ui->qlv_mobilities->setLocale(DoubleToStringConvertor::locale());
}

void HyperboleFitWidget::onRedrawClicked()
{
  emit redrawDataSeries();
}

void HyperboleFitWidget::onRemoveAnalyteClicked()
{
  QMessageBox::StandardButton reply;
  QMap<int, QVariant> itemData;
  QString name;
  const QModelIndex &idx = ui->qlv_analytes->currentIndex();

  if (!idx.isValid())
    return;

  itemData = ui->qlv_analytes->model()->itemData(idx);
  if (!itemData.contains(Qt::DisplayRole))
    return;
  name = itemData[Qt::DisplayRole].toString();


  reply = QMessageBox::question(this, tr("Remove analyte?"), QString(tr("Really remove the analyte \"%1\"?")).arg(name),
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    const QModelIndex srcidx = m_analytesSortProxy.mapToSource(idx);
    emit removeAnalyte(srcidx);
    m_mobilitiesSortProxy.sort(0);
  }
}

void HyperboleFitWidget::onRemoveConcentrationClicked()
{
  QMessageBox::StandardButton reply;
  QMap<int, QVariant> itemData;
  double c;
  bool ok;
  const QModelIndex &idx = ui->qlv_concentrations->currentIndex();

  if (!idx.isValid())
    return;

  itemData = ui->qlv_concentrations->model()->itemData(idx);
  if (!itemData.contains(Qt::DisplayRole))
    return;
  c = itemData[Qt::DisplayRole].toDouble(&ok);
  if (!ok)
    return;

  reply = QMessageBox::question(this, tr("Remove concentration?"), QString(tr("Really remove concentraiton \"%1\"?")).arg(c),
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    const QModelIndex srcidx = m_concentrationsSortProxy.mapToSource(idx);
    emit removeConcentration(srcidx);
    m_concentrationsSortProxy.sort(0);
  }
}

void HyperboleFitWidget::onRemoveMobilityClicked()
{
  const QModelIndex srcidx = m_mobilitiesSortProxy.mapToSource(ui->qlv_mobilities->currentIndex());
  emit removeMobility(srcidx);
}

void HyperboleFitWidget::onShowChartHorizontalMarker()
{
  const bool visible = ui->qcb_chartHorizontalMarker->checkState() == Qt::Checked;

  ui->qle_chartHorizontalMarker->setEnabled(visible);

  emit showChartMarker(HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER, visible, ui->qle_chartHorizontalMarker->text());
}

void HyperboleFitWidget::onShowChartVerticalAMarker()
{
  const bool visible = ui->qcb_chartVerticalAMarker->checkState() == Qt::Checked;

  ui->qle_chartVerticalAMarker->setEnabled(visible);

  emit showChartMarker(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, visible, ui->qle_chartVerticalAMarker->text());
}

void HyperboleFitWidget::onShowChartVerticalBMarker()
{
  const bool visible = ui->qcb_chartVerticalBMarker->checkState() == Qt::Checked;

  ui->qle_chartVerticalBMarker->setEnabled(visible);

  emit showChartMarker(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, visible, ui->qle_chartVerticalBMarker->text());
}

void HyperboleFitWidget::onStatBothClicked()
{
  emit doStats(HyperboleStats::Intervals::BOTH);
}

void HyperboleFitWidget::onStatLeftClicked()
{
  emit doStats(HyperboleStats::Intervals::LEFT);
}

void HyperboleFitWidget::onStatRightClicked()
{
  emit doStats(HyperboleStats::Intervals::RIGHT);
}

void HyperboleFitWidget::onStatModeActivated(const int idx)
{
  Q_UNUSED(idx);

  emit statModeChanged(ui->qcbox_statData->currentData(Qt::UserRole + 1));
}

void HyperboleFitWidget::onStatUnitsActivated(const int idx)
{
  Q_UNUSED(idx);

  emit statUnitsChanged(ui->qcbox_statUnits->currentData(Qt::UserRole + 1));
}

void HyperboleFitWidget::setAnalyteNamesModel(AbstractMapperModel<QString, HyperboleFitParameters::String> *model)
{
  m_analyteNamesMapper.setModel(model);

  m_analyteNamesMapper.addMapping(ui->qle_analyteAName, model->indexFromItem(HyperboleFitParameters::String::ANALYTE_A));
  m_analyteNamesMapper.addMapping(ui->qle_analyteBName, model->indexFromItem(HyperboleFitParameters::String::ANALYTE_B));
  m_analyteNamesMapper.toFirst();
}

void HyperboleFitWidget::setAnalytesModel(QAbstractItemModel *model)
{
  m_analytesSortProxy.setSourceModel(model);
  ui->qlv_analytes->setModel(&m_analytesSortProxy);

  m_analytesSortProxy.setSortRole(Qt::UserRole + 1);
}

void HyperboleFitWidget::setConcentrationsModel(QAbstractItemModel *model)
{
  m_concentrationsSortProxy.setSourceModel(model);
  ui->qlv_concentrations->setModel(&m_concentrationsSortProxy);

  m_concentrationsSortProxy.setSortRole(Qt::UserRole + 1);
}

void HyperboleFitWidget::setFitFixedModel(AbstractMapperModel<bool, HyperboleFitParameters::Boolean> *model)
{
  m_fitFixedMapper.setModel(model);

  m_fitFixedMapper.addMapping(ui->qcb_fixedCsMobility, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedCsMobilityB, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_B));
  m_fitFixedMapper.addMapping(ui->qcb_fixedFreeMobility, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_MOBILITY_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedFreeMobilityB, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_MOBILITY_B));
  m_fitFixedMapper.addMapping(ui->qcb_fixedKCS, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_K_CS_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedKCSB, model->indexFromItem(HyperboleFitParameters::Boolean::FIXED_K_CS_B));
  m_fitFixedMapper.toFirst();
}

void HyperboleFitWidget::setFitFloatModel(AbstractMapperModel<double, HyperboleFitParameters::Floating> *model)
{
  m_fitFloatMapper.setModel(model);
  m_fitFloatMapper.setItemDelegate(&m_floatingDelegate);

  m_fitFloatMapper.addMapping(ui->qle_epsilon, model->indexFromItem(HyperboleFitParameters::Floating::EPSILON));
  m_fitFloatMapper.addMapping(ui->qle_viscositySlope, model->indexFromItem(HyperboleFitParameters::Floating::VISCOSITY_SLOPE));
  m_fitFloatMapper.toFirst();
}

void HyperboleFitWidget::setFitIntModel(AbstractMapperModel<int, HyperboleFitParameters::Int> *model)
{
  m_fitIntMapper.setModel(model);

  m_fitIntMapper.addMapping(ui->qle_maxIters, model->indexFromItem(HyperboleFitParameters::Int::MAX_ITERATIONS));
  m_fitIntMapper.toFirst();
}

void HyperboleFitWidget::setFitResultsModel(AbstractMapperModel<double, HyperboleFitResults::Floating> *model)
{
  m_fitResultsMapper.setModel(model);
  m_fitResultsMapper.setItemDelegate(&m_floatingDelegate);

  m_fitResultsMapper.addMapping(ui->qle_csMobility, model->indexFromItem(HyperboleFitResults::Floating::MOBILITY_CS_A));
  m_fitResultsMapper.addMapping(ui->qle_csMobilityB, model->indexFromItem(HyperboleFitResults::Floating::MOBILITY_CS_B));
  m_fitResultsMapper.addMapping(ui->qle_freeMobility, model->indexFromItem(HyperboleFitResults::Floating::MOBILITY_A));
  m_fitResultsMapper.addMapping(ui->qle_freeMobilityB, model->indexFromItem(HyperboleFitResults::Floating::MOBILITY_B));
  m_fitResultsMapper.addMapping(ui->qle_KCS, model->indexFromItem(HyperboleFitResults::Floating::K_CS_A));
  m_fitResultsMapper.addMapping(ui->qle_KCSB, model->indexFromItem(HyperboleFitResults::Floating::K_CS_B));
  m_fitResultsMapper.addMapping(ui->qle_rss, model->indexFromItem(HyperboleFitResults::Floating::SIGMA));
  m_fitResultsMapper.addMapping(ui->qle_iterations, model->indexFromItem(HyperboleFitResults::Floating::ITERATIONS));
  m_fitResultsMapper.toFirst();
}

void HyperboleFitWidget::setFitModeModel(QAbstractItemModel *model)
{
  ui->qcbox_fitMode->setModel(model);
}

void HyperboleFitWidget::setMobilitiesModel(QAbstractItemModel *model)
{
  m_mobilitiesSortProxy.setSourceModel(model);
  ui->qlv_mobilities->setModel(&m_mobilitiesSortProxy);

  m_mobilitiesSortProxy.setSortRole(Qt::UserRole + 1);
}

void HyperboleFitWidget::setStatModeModel(QAbstractItemModel *model)
{
  ui->qcbox_statData->setModel(model);
}

void HyperboleFitWidget::setStatUnitsModel(QAbstractItemModel *model)
{
  ui->qcbox_statUnits->setModel(model);
}
