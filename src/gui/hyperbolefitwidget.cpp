#include "hyperbolefitwidget.h"
#include "ui_hyperbolefitwidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <limits>
#include "../hyperbolefittingwidgetconnector.h"

#include <QDebug>

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

  connect(ui->qpb_bothConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatBothClicked);
  connect(ui->qpb_leftConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatLeftClicked);
  connect(ui->qpb_rightConfIntr, &QPushButton::clicked, this, &HyperboleFitWidget::onStatRightClicked);

  connect(ui->qpb_estimate, &QPushButton::clicked, this, &HyperboleFitWidget::onEstimateClicked);
  connect(ui->qpb_fit, &QPushButton::clicked, this, &HyperboleFitWidget::onFitClicked);
  connect(ui->qpb_redrawPoints, &QPushButton::clicked, this, &HyperboleFitWidget::onRedrawClicked);

  ui->qle_analyteAName->setDisabled(true);
  ui->qle_analyteBName->setDisabled(true);
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
        return;
      }
    } else
      return;
  }
}

void HyperboleFitWidget::onAddConcentrationClicked()
{
  QInputDialog dlg(this);
  int ret;

  dlg.setLabelText(tr("Enter concentration"));
  dlg.setInputMode(QInputDialog::DoubleInput);
  dlg.setDoubleMinimum(0.0);
  dlg.setDoubleMaximum(std::numeric_limits<double>::max());

  ret = dlg.exec();
  if (ret == QDialog::Accepted)
    emit addConcentration(dlg.doubleValue());

  return;
}

void HyperboleFitWidget::onAddMobilityClicked()
{
  QInputDialog dlg(this);
  int ret;

  dlg.setLabelText(tr("Enter migrationtime"));
  dlg.setInputMode(QInputDialog::DoubleInput);
  dlg.setDoubleMinimum(0.0);
  dlg.setDoubleMaximum(std::numeric_limits<double>::max());

  ret = dlg.exec();
  if (ret == QDialog::Accepted)
    emit addMobility(dlg.doubleValue());

  return;
}

void HyperboleFitWidget::onAnalyteListClicked()
{
  QModelIndexList list = ui->qlv_analytes->selectionModel()->selectedIndexes();
  QModelIndexList list2;

  /* :-P */
  if (list.size() > 2) {
    ui->qlv_analytes->clearSelection();
    for (int idx = 0; idx < 2; idx++) {
      ui->qlv_analytes->setCurrentIndex(list.at(idx));
      list2.push_back(list.at(idx));
    }

    emit analyteSwitched(list2);
  } else
    emit analyteSwitched(list);
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

void HyperboleFitWidget::onConcentrationsListClicked(const QModelIndex &idx)
{
  emit concentrationSwitched(idx);
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
  if (reply == QMessageBox::Yes)
    emit removeAnalyte(idx);
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
  if (reply == QMessageBox::Yes)
    emit removeConcentration(idx);
}

void HyperboleFitWidget::onRemoveMobilityClicked()
{
  emit removeMobility(ui->qlv_mobilities->currentIndex());
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
  ui->qlv_analytes->setModel(model);
}

void HyperboleFitWidget::setConcentrationsModel(QAbstractItemModel *model)
{
  ui->qlv_concentrations->setModel(model);
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
  ui->qlv_mobilities->setModel(model);
}

void HyperboleFitWidget::setStatModeModel(QAbstractItemModel *model)
{
  ui->qcbox_statData->setModel(model);
}

void HyperboleFitWidget::setStatUnitsModel(QAbstractItemModel *model)
{
  ui->qcbox_statUnits->setModel(model);
}
