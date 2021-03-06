#include "hyperbolafitwidget.h"
#include "ui_hyperbolafitwidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QStandardItem>
#include <limits>
#include "../hyperbolafittingwidgetconnector.h"
#include "../doubletostringconvertor.h"

HyperbolaFitWidget::HyperbolaFitWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::HyperbolaFitWidget)
{
  ui->setupUi(this);

  connect(ui->qpb_addAnalyte, &QPushButton::clicked, this, &HyperbolaFitWidget::onAddAnalyteClicked);
  connect(ui->qpb_removeAnalyte, &QPushButton::clicked, this, &HyperbolaFitWidget::onRemoveAnalyteClicked);
  connect(ui->qlv_analytes, &QListView::doubleClicked, this, &HyperbolaFitWidget::onAnalyteListDoubleClicked);

  connect(ui->qpb_addConcentration, &QPushButton::clicked, this, &HyperbolaFitWidget::onAddConcentrationClicked);
  connect(ui->qpb_removeConcentration, &QPushButton::clicked, this, &HyperbolaFitWidget::onRemoveConcentrationClicked);
  connect(ui->qlv_concentrations, &QListView::doubleClicked, this, &HyperbolaFitWidget::onConcentrationsListDoubleClicked);

  connect(ui->qpb_addMobility, &QPushButton::clicked, this, &HyperbolaFitWidget::onAddMobilityClicked);
  connect(ui->qpb_removeMobility, &QPushButton::clicked, this, &HyperbolaFitWidget::onRemoveMobilityClicked);
  connect(ui->qlv_mobilities, &QListView::doubleClicked, this, &HyperbolaFitWidget::onMobilitiesListDoubleClicked);

  connect(ui->qcbox_fitMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperbolaFitWidget::onFitModeActivated);
  connect(ui->qcbox_statData, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperbolaFitWidget::onStatModeActivated);
  connect(ui->qcbox_statUnits, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &HyperbolaFitWidget::onStatUnitsActivated);
  connect(ui->qrb_leftAnalyte, &QRadioButton::clicked, this, &HyperbolaFitWidget::onStatsForAnalyteChanged);
  connect(ui->qrb_rightAnalyte, &QRadioButton::clicked, this, &HyperbolaFitWidget::onStatsForAnalyteChanged);

  connect(ui->qpb_bothConfIntr, &QPushButton::clicked, this, &HyperbolaFitWidget::onStatBothClicked);
  connect(ui->qpb_leftConfIntr, &QPushButton::clicked, this, &HyperbolaFitWidget::onStatLeftClicked);
  connect(ui->qpb_rightConfIntr, &QPushButton::clicked, this, &HyperbolaFitWidget::onStatRightClicked);
  connect(ui->qcb_chartHorizontalMarker, &QCheckBox::clicked, this, &HyperbolaFitWidget::onShowChartHorizontalMarker);
  connect(ui->qcb_chartVerticalAMarker, &QCheckBox::clicked, this, &HyperbolaFitWidget::onShowChartVerticalAMarker);
  connect(ui->qcb_chartVerticalBMarker, &QCheckBox::clicked, this, &HyperbolaFitWidget::onShowChartVerticalBMarker);
  connect(ui->qle_chartHorizontalMarker, &QLineEdit::textChanged, this, &HyperbolaFitWidget::onChartHorizontalMarkerValueChanged);
  connect(ui->qle_chartVerticalAMarker, &QLineEdit::textChanged, this, &HyperbolaFitWidget::onChartVerticalAMarkerValueChanged);
  connect(ui->qle_chartVerticalBMarker, &QLineEdit::textChanged, this, &HyperbolaFitWidget::onChartVerticalBMarkerValueChanged);
  connect(ui->qpb_chartHorizontalMarkerLeftItsc, &QPushButton::clicked, this, &HyperbolaFitWidget::onChartHorizontalMarkerLeftItscClicked);
  connect(ui->qpb_chartHorizontalMarkerRightItsc, &QPushButton::clicked, this, &HyperbolaFitWidget::onChartHorizontalMarkerRightItscClicked);
  connect(ui->qpb_chartVerticalAMarkerItsc,&QPushButton::clicked, this, &HyperbolaFitWidget::onChartVerticalAMarkerItscClicked);
  connect(ui->qpb_chartVerticalBMarkerItsc,&QPushButton::clicked, this, &HyperbolaFitWidget::onChartVerticalBMarkerItscClicked);

  connect(ui->qpb_estimate, &QPushButton::clicked, this, &HyperbolaFitWidget::onEstimateClicked);
  connect(ui->qpb_fit, &QPushButton::clicked, this, &HyperbolaFitWidget::onFitClicked);
  connect(ui->qpb_redrawPoints, &QPushButton::clicked, this, &HyperbolaFitWidget::onRedrawClicked);
  connect(ui->qpb_clearHyperbola, &QPushButton::clicked, this, &HyperbolaFitWidget::onClearHyperbolaClicked);

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

HyperbolaFitWidget::~HyperbolaFitWidget()
{
  delete ui;
}

void HyperbolaFitWidget::connectToAccumulator(QObject *dac)
{
  HyperbolaFittingWidgetConnector::connectAll(this, dac);

  connect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperbolaFitWidget::onAnalyteListClicked);
  connect(ui->qlv_concentrations->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &HyperbolaFitWidget::onConcentrationsListClicked);
}

void HyperbolaFitWidget::onAddAnalyteClicked()
{
  QInputDialog dlg(this);
  int ret;
  QModelIndex idx;

  dlg.setLabelText(tr("Input name of the analyte"));

  while (true) {
    ret = dlg.exec();

    if (ret == QDialog::Accepted) {
      QString name = dlg.textValue();
      if (name.length() > 0) {
        emit addAnalyte(name, idx);

        if (idx.isValid()) {
          m_analytesSortProxy.sort(0);
          const QModelIndex proxyIdx = m_analytesSortProxy.mapFromSource(idx);
          ui->qlv_analytes->setCurrentIndex(proxyIdx);
        }
        return;
      }
    } else
      return;
  }
}

void HyperbolaFitWidget::onAddConcentrationClicked()
{
  QInputDialog dlg(this);
  QModelIndex idx;

  dlg.setLabelText(tr("Enter concentration"));
  dlg.setInputMode(QInputDialog::TextInput);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double d = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }
    emit addConcentration(d, idx);

    if (idx.isValid()) {
      m_concentrationsSortProxy.sort(0);
      const QModelIndex proxyIdx = m_concentrationsSortProxy.mapFromSource(idx);
      ui->qlv_concentrations->setCurrentIndex(proxyIdx);
    }
    break;
  }

  return;
}

void HyperbolaFitWidget::onAddMobilityClicked()
{
  QInputDialog dlg(this);
  QModelIndex idx;

  dlg.setLabelText(tr("Enter mobility of the analyte"));
  dlg.setInputMode(QInputDialog::TextInput);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double d = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }
    emit addMobility(d, idx);

    if (idx.isValid()) {
      m_mobilitiesSortProxy.sort(0);
      const QModelIndex proxyIdx = m_mobilitiesSortProxy.mapFromSource(idx);
      ui->qlv_mobilities->setCurrentIndex(proxyIdx);
    }
    break;
  }

  return;
}

void HyperbolaFitWidget::onAnalyteListClicked()
{
  QModelIndexList list = ui->qlv_analytes->selectionModel()->selectedIndexes();
  QModelIndexList list2;

  /* :-P */
  disconnect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperbolaFitWidget::onAnalyteListClicked);
  if (list.size() > 0) {
    ui->qlv_analytes->clearSelection();
    for (int idx = 0; idx < (list.size() > 2 ? 2 : list.size()); idx++) {
      ui->qlv_analytes->setCurrentIndex(list.at(idx));
      list2.push_back(m_analytesSortProxy.mapToSource(list.at(idx)));
    }

    emit analyteSwitched(list2);
  }
  connect(ui->qlv_analytes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HyperbolaFitWidget::onAnalyteListClicked);
}

void HyperbolaFitWidget::onAnalyteListDoubleClicked(const QModelIndex &idx)
{
  QInputDialog dlg(this);
  QVariant internalId;
  QString oldName;
  int ret;
  QModelIndex srcidx;

  if (!idx.isValid())
    return;

  internalId = ui->qlv_analytes->model()->data(idx, Qt::UserRole + 1);
  oldName = ui->qlv_analytes->model()->data(idx, Qt::DisplayRole).toString();

  srcidx = m_analytesSortProxy.mapToSource(idx);

  dlg.setLabelText(tr("Input a new name of the analyte"));
  dlg.setTextValue(oldName);

  while (true) {
    ret = dlg.exec();

    if (ret == QDialog::Accepted) {
      QString newName = dlg.textValue();
      if (newName.length() > 0) {
        emit renameAnalyte(internalId, newName, srcidx.row());

        /* Model might have resorted, make sure we select the correct analyte again */
        const QModelIndex newIdx = m_analytesSortProxy.mapFromSource(srcidx);
        ui->qlv_analytes->setCurrentIndex(newIdx);
        onAnalyteListClicked();
        return;
      }
    } else
      return;
  }
}

void HyperbolaFitWidget::onChartHorizontalMarkerLeftItscClicked()
{
  emit chartHorizontalMarkerIntersection(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
}

void HyperbolaFitWidget::onChartHorizontalMarkerRightItscClicked()
{
  emit chartHorizontalMarkerIntersection(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
}

void HyperbolaFitWidget::onChartVerticalAMarkerItscClicked()
{
  emit chartVerticalMarkerIntersection(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
}

void HyperbolaFitWidget::onChartVerticalBMarkerItscClicked()
{
  emit chartVerticalMarkerIntersection(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
}

void HyperbolaFitWidget::onChartHorizontalMarkerIntersectionSet(const double d)
{
  ui->qle_chartHorizontalMarker->setText(DoubleToStringConvertor::convert(d));
}

void HyperbolaFitWidget::onChartVerticalMarkerIntersectionSet(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d)
{
  switch (marker) {
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    ui->qle_chartVerticalAMarker->setText(DoubleToStringConvertor::convert(d));
    break;
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    ui->qle_chartVerticalBMarker->setText(DoubleToStringConvertor::convert(d));
    break;
  default:
    break;
  }
}

void HyperbolaFitWidget::onChartHorizontalMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER, d);
}

void HyperbolaFitWidget::onChartVerticalAMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, d);
}

void HyperbolaFitWidget::onChartVerticalBMarkerValueChanged(const QString &value)
{
  bool ok;
  const double d = DoubleToStringConvertor::back(value, &ok);

  if (ok)
    emit chartMarkerValueChanged(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, d);
}

void HyperbolaFitWidget::onClearHyperbolaClicked()
{
  emit clearHyperbola();
}

void HyperbolaFitWidget::onConcentrationsListClicked(const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  const QModelIndex srcidx = m_concentrationsSortProxy.mapToSource(idx);
  emit concentrationSwitched(srcidx);
}

void HyperbolaFitWidget::onConcentrationsListDoubleClicked(const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  QInputDialog dlg(this);
  const QModelIndex srcidx = m_concentrationsSortProxy.mapToSource(idx);
  const QString oldConc = ui->qlv_concentrations->model()->data(idx, Qt::DisplayRole).toString();

  dlg.setLabelText(tr("Edit concentration"));
  dlg.setTextValue(oldConc);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double newConc = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }

    emit editConcentration(newConc, srcidx);
    const QModelIndex newIdx = m_concentrationsSortProxy.mapFromSource(srcidx);
    ui->qlv_concentrations->setCurrentIndex(newIdx);

    break;
  }
}

void HyperbolaFitWidget::onMobilitiesListDoubleClicked(const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  QInputDialog dlg(this);
  const QModelIndex srcidx = m_mobilitiesSortProxy.mapToSource(idx);
  const QString oldMobility = ui->qlv_mobilities->model()->data(idx).toString();

  dlg.setLabelText(tr("Edit mobility"));
  dlg.setTextValue(oldMobility);

  while (dlg.exec() == QDialog::Accepted) {
    bool ok;
    const double u = DoubleToStringConvertor::back(dlg.textValue(), &ok);
    if (!ok) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Non-numeric value"));
      continue;
    }

    emit editMobility(u, srcidx);
    const QModelIndex newIdx = m_mobilitiesSortProxy.mapFromSource(srcidx);
    ui->qlv_mobilities->setCurrentIndex(newIdx);

    break;
  }
}

void HyperbolaFitWidget::onStatsForAnalyteChanged()
{
  if (ui->qrb_leftAnalyte->isChecked())
    emit statsForAnalyteChanged(HyperbolaFittingEngineMsgs::AnalyteId::ANALYTE_A);
  if (ui->qrb_rightAnalyte->isChecked())
    emit statsForAnalyteChanged(HyperbolaFittingEngineMsgs::AnalyteId::ANALYTE_B);
}

void HyperbolaFitWidget::onEnableDoubleFit(const bool enable)
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
    onStatsForAnalyteChanged();
}

void HyperbolaFitWidget::onEstimateClicked()
{
  emit doEstimate();
}

void HyperbolaFitWidget::onFitClicked()
{
  emit doFit();
}

void HyperbolaFitWidget::onFitModeActivated(const int idx)
{
  Q_UNUSED(idx);

  emit fitModeChanged(ui->qcbox_fitMode->currentData(Qt::UserRole + 1));
}

void HyperbolaFitWidget::onNumberFormatChanged(const QLocale *oldLocale)
{
  Q_UNUSED(oldLocale);

  ui->qlv_analytes->setLocale(DoubleToStringConvertor::locale());
  ui->qlv_concentrations->setLocale(DoubleToStringConvertor::locale());
  ui->qlv_mobilities->setLocale(DoubleToStringConvertor::locale());

  m_analytesSortProxy.setCurrentLocale(DoubleToStringConvertor::locale());
  m_concentrationsSortProxy.setCurrentLocale(DoubleToStringConvertor::locale());
  m_mobilitiesSortProxy.setCurrentLocale(DoubleToStringConvertor::locale());
}

void HyperbolaFitWidget::onRedrawClicked()
{
  emit redrawDataSeries();
}

void HyperbolaFitWidget::onRemoveAnalyteClicked()
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
    m_analytesSortProxy.sort(0);
  }
}

void HyperbolaFitWidget::onRemoveConcentrationClicked()
{
  QMessageBox::StandardButton reply;
  QMap<int, QVariant> itemData;
  const QModelIndex &idx = ui->qlv_concentrations->currentIndex();

  if (!idx.isValid())
    return;

  itemData = ui->qlv_concentrations->model()->itemData(idx);
  if (!itemData.contains(Qt::DisplayRole))
    return;

  const QString numStr = itemData[Qt::DisplayRole].toString();

  reply = QMessageBox::question(this, tr("Remove concentration?"), QString(tr("Really remove concentraiton \"%1\"?")).arg(numStr),
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    const QModelIndex srcidx = m_concentrationsSortProxy.mapToSource(idx);
    emit removeConcentration(srcidx);
    m_concentrationsSortProxy.sort(0);
  }
}

void HyperbolaFitWidget::onRemoveMobilityClicked()
{
  const QModelIndex srcidx = m_mobilitiesSortProxy.mapToSource(ui->qlv_mobilities->currentIndex());
  emit removeMobility(srcidx);
  m_mobilitiesSortProxy.sort(0);
}

void HyperbolaFitWidget::onShowChartHorizontalMarker()
{
  const bool visible = ui->qcb_chartHorizontalMarker->checkState() == Qt::Checked;

  ui->qle_chartHorizontalMarker->setEnabled(visible);

  emit showChartMarker(HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER, visible, ui->qle_chartHorizontalMarker->text());
}

void HyperbolaFitWidget::onShowChartVerticalAMarker()
{
  const bool visible = ui->qcb_chartVerticalAMarker->checkState() == Qt::Checked;

  ui->qle_chartVerticalAMarker->setEnabled(visible);

  emit showChartMarker(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, visible, ui->qle_chartVerticalAMarker->text());
}

void HyperbolaFitWidget::onShowChartVerticalBMarker()
{
  const bool visible = ui->qcb_chartVerticalBMarker->checkState() == Qt::Checked;

  ui->qle_chartVerticalBMarker->setEnabled(visible);

  emit showChartMarker(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, visible, ui->qle_chartVerticalBMarker->text());
}

void HyperbolaFitWidget::onSortLists()
{
  m_concentrationsSortProxy.sort(0);
  m_mobilitiesSortProxy.sort(0);
}

void HyperbolaFitWidget::onStatBothClicked()
{
  emit doStats(HyperbolaStats::Intervals::BOTH);
}

void HyperbolaFitWidget::onStatLeftClicked()
{
  emit doStats(HyperbolaStats::Intervals::LEFT);
}

void HyperbolaFitWidget::onStatRightClicked()
{
  emit doStats(HyperbolaStats::Intervals::RIGHT);
}

void HyperbolaFitWidget::onStatModeActivated(const int idx)
{
  Q_UNUSED(idx);

  emit statModeChanged(ui->qcbox_statData->currentData(Qt::UserRole + 1));
}

void HyperbolaFitWidget::onStatUnitsActivated(const int idx)
{
  Q_UNUSED(idx);

  emit statUnitsChanged(ui->qcbox_statUnits->currentData(Qt::UserRole + 1));
}

void HyperbolaFitWidget::setAnalyteNamesModel(AbstractMapperModel<QString, HyperbolaFitParameters::String> *model)
{
  m_analyteNamesMapper.setModel(model);

  m_analyteNamesMapper.addMapping(ui->qle_analyteAName, model->indexFromItem(HyperbolaFitParameters::String::ANALYTE_A));
  m_analyteNamesMapper.addMapping(ui->qle_analyteBName, model->indexFromItem(HyperbolaFitParameters::String::ANALYTE_B));
  m_analyteNamesMapper.toFirst();
}

void HyperbolaFitWidget::setAnalytesModel(QAbstractItemModel *model)
{
  m_analytesSortProxy.setSourceModel(model);
  ui->qlv_analytes->setModel(&m_analytesSortProxy);

  m_analytesSortProxy.setSortRole(Qt::UserRole + 1);
  m_analytesSortProxy.setSortCaseSensitivity(Qt::CaseSensitive);
  m_analytesSortProxy.setSortLocaleAware(true);
}

void HyperbolaFitWidget::setConcentrationsModel(QAbstractItemModel *model)
{
  m_concentrationsSortProxy.setSourceModel(model);
  ui->qlv_concentrations->setModel(&m_concentrationsSortProxy);

  m_concentrationsSortProxy.setSortRole(Qt::UserRole + 1);
  m_concentrationsSortProxy.setSortLocaleAware(true);
}

void HyperbolaFitWidget::setFitFixedModel(AbstractMapperModel<bool, HyperbolaFitParameters::Boolean> *model)
{
  m_fitFixedMapper.setModel(model);

  m_fitFixedMapper.addMapping(ui->qcb_fixedCsMobility, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedCsMobilityB, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_B));
  m_fitFixedMapper.addMapping(ui->qcb_fixedFreeMobility, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedFreeMobilityB, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_B));
  m_fitFixedMapper.addMapping(ui->qcb_fixedKCS, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_K_CS_A));
  m_fitFixedMapper.addMapping(ui->qcb_fixedKCSB, model->indexFromItem(HyperbolaFitParameters::Boolean::FIXED_K_CS_B));
  m_fitFixedMapper.toFirst();
}

void HyperbolaFitWidget::setFitFloatModel(AbstractMapperModel<double, HyperbolaFitParameters::Floating> *model)
{
  m_fitFloatMapper.setModel(model);
  m_fitFloatMapper.setItemDelegate(&m_floatingDelegate);

  m_fitFloatMapper.addMapping(ui->qle_epsilon, model->indexFromItem(HyperbolaFitParameters::Floating::EPSILON));
  m_fitFloatMapper.addMapping(ui->qle_viscositySlope, model->indexFromItem(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE));
  m_fitFloatMapper.toFirst();
}

void HyperbolaFitWidget::setFitIntModel(AbstractMapperModel<int, HyperbolaFitParameters::Int> *model)
{
  m_fitIntMapper.setModel(model);

  m_fitIntMapper.addMapping(ui->qle_maxIters, model->indexFromItem(HyperbolaFitParameters::Int::MAX_ITERATIONS));
  m_fitIntMapper.toFirst();
}

void HyperbolaFitWidget::setFitResultsModel(AbstractMapperModel<double, HyperbolaFitResults::Floating> *model)
{
  m_fitResultsMapper.setModel(model);
  m_fitResultsMapper.setItemDelegate(&m_floatingDelegate);

  m_fitResultsMapper.addMapping(ui->qle_csMobility, model->indexFromItem(HyperbolaFitResults::Floating::MOBILITY_CS_A));
  m_fitResultsMapper.addMapping(ui->qle_csMobilityB, model->indexFromItem(HyperbolaFitResults::Floating::MOBILITY_CS_B));
  m_fitResultsMapper.addMapping(ui->qle_freeMobility, model->indexFromItem(HyperbolaFitResults::Floating::MOBILITY_A));
  m_fitResultsMapper.addMapping(ui->qle_freeMobilityB, model->indexFromItem(HyperbolaFitResults::Floating::MOBILITY_B));
  m_fitResultsMapper.addMapping(ui->qle_KCS, model->indexFromItem(HyperbolaFitResults::Floating::K_CS_A));
  m_fitResultsMapper.addMapping(ui->qle_KCSB, model->indexFromItem(HyperbolaFitResults::Floating::K_CS_B));
  m_fitResultsMapper.addMapping(ui->qle_rss, model->indexFromItem(HyperbolaFitResults::Floating::SIGMA));
  m_fitResultsMapper.addMapping(ui->qle_iterations, model->indexFromItem(HyperbolaFitResults::Floating::ITERATIONS));
  m_fitResultsMapper.toFirst();
}

void HyperbolaFitWidget::setFitModeModel(QAbstractItemModel *model)
{
  ui->qcbox_fitMode->setModel(model);
}

void HyperbolaFitWidget::setMobilitiesModel(QAbstractItemModel *model)
{
  m_mobilitiesSortProxy.setSourceModel(model);
  ui->qlv_mobilities->setModel(&m_mobilitiesSortProxy);

  m_mobilitiesSortProxy.setSortRole(Qt::UserRole + 1);
  m_mobilitiesSortProxy.setSortLocaleAware(true);
}

void HyperbolaFitWidget::setStatModeModel(QAbstractItemModel *model)
{
  ui->qcbox_statData->setModel(model);
}

void HyperbolaFitWidget::setStatUnitsModel(QAbstractItemModel *model)
{
  ui->qcbox_statUnits->setModel(model);
}
