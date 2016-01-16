#include "evaluationwidget.h"
#include "ui_evaluationwidget.h"
#include "../evaluationwidgetconnector.h"

EvaluationWidget::EvaluationWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::EvaluationWidget)
{
  m_commonParametersWidget = new CommonParametersWidget(this);
  m_evaluatedPeaksWidget = new EvaluatedPeaksWidget(this);
  m_evaluationParametersAutoMapper = new QDataWidgetMapper(this);
  m_evaluationParametersBooleanMapper = new QDataWidgetMapper(this);
  m_evaluationParametersFloatingMapper = new QDataWidgetMapper(this);
  m_evaluationResultsMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitFixedMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitIntMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitMapper = new QDataWidgetMapper(this);

  m_evaluationResultsMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

  ui->setupUi(this);
  ui->qw_commonParametersUpperWidget->setLayout(new QVBoxLayout(this));
  ui->qw_commonParametersUpperWidget->layout()->setMargin(0);
  ui->qw_commonParametersUpperWidget->layout()->addWidget(m_commonParametersWidget);
  ui->qw_commonParametersUpperWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  ui->qw_commonParametersUpperWidget->setMinimumWidth(m_commonParametersWidget->width());


  ui->qw_evaluatedPeaksUpperWidget->setLayout(new QVBoxLayout(this));
  ui->qw_evaluatedPeaksUpperWidget->layout()->setMargin(0);
  ui->qw_evaluatedPeaksUpperWidget->layout()->addWidget(m_evaluatedPeaksWidget);

  connect(ui->qpb_defaultFinderParameters, &QPushButton::clicked, this, &EvaluationWidget::onDefaultFinderParametersClicked);
  connect(ui->qpb_defaultPeakProperties, &QPushButton::clicked, this, &EvaluationWidget::onDefaultPeakPropertiesClicked);
  connect(ui->qcbox_baselineAlgorithm, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onBaselineComboBoxChanged);
  connect(ui->qcbox_showWindow, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onShowWindowComboBoxChanged);
  connect(ui->qcbox_windowUnits, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onWindowUnitsComboBoxChanged);
  connect(ui->qcbox_files, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onFileComboBoxChanged);
  connect(ui->qpb_closeFile, &QPushButton::clicked, this, &EvaluationWidget::onCloseFileClicked);
  connect(ui->qpb_findPeak, &QPushButton::clicked, this, &EvaluationWidget::onFindPeaksClicked);
  connect(ui->qpb_doHvlFit, &QPushButton::clicked, this, &EvaluationWidget::onDoHvlFitClicked);
  connect(ui->qpb_replotHvl, &QPushButton::clicked, this, &EvaluationWidget::onReplotHvl);
}

EvaluationWidget::~EvaluationWidget()
{
  delete ui;
}

void EvaluationWidget::connectToAccumulator(QObject *dac)
{
  EvaluationWidgetConnector::connectAll(this, dac);
  m_commonParametersWidget->connectToAccumulator(dac);
  m_evaluatedPeaksWidget->connectToAccumulator(dac);
}

void EvaluationWidget::onBaselineComboBoxChanged(const int idx){
  QModelIndex midx = ui->qcbox_baselineAlgorithm->model()->index(idx, 0);
  EvaluationParametersItems::ComboBaselineAlgorithm val = ui->qcbox_baselineAlgorithm->model()->data(midx, Qt::UserRole + 1).value<EvaluationParametersItems::ComboBaselineAlgorithm>();
  emit comboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                              EvaluationParametersItems::index(val)));
}

void EvaluationWidget::onCloseFileClicked()
{
  emit closeFile(ui->qcbox_files->currentIndex());
}

void EvaluationWidget::onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier)
{
  switch (notifier.id) {
  case EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM:
    ui->qcbox_baselineAlgorithm->setCurrentIndex(notifier.value);
    break;
  case EvaluationEngineMsgs::ComboBox::DATA_FILES:
    ui->qcbox_files->setCurrentIndex(notifier.value);
    break;
  case EvaluationEngineMsgs::ComboBox::SHOW_WINDOW:
    ui->qcbox_showWindow->setCurrentIndex(notifier.value);
    break;
  case EvaluationEngineMsgs::ComboBox::WINDOW_UNITS:
    ui->qcbox_windowUnits->setCurrentIndex(notifier.value);
    break;
  default:
    break;
  }
}

void EvaluationWidget::onDefaultFinderParametersClicked()
{
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::FINDER_PARAMETERS);
}

void EvaluationWidget::onDefaultPeakPropertiesClicked()
{
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::PEAK_PROPERTIES);
}

void EvaluationWidget::onDoHvlFitClicked()
{
  emit doHvlFit();
}

void EvaluationWidget::onEvaluationAutoModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  Q_UNUSED(bottomRight);

  if (!roles.contains(Qt::EditRole))
    return;

  for (int idx = topLeft.column(); idx <= bottomRight.column(); idx++) {
    const AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model = dynamic_cast<AbstractMapperModel<bool, EvaluationParametersItems::Auto> *>(m_evaluationParametersAutoMapper->model());
    Q_ASSERT(model != nullptr);

    QModelIndex index = model->index(0, idx);
    bool value = model->data(index, Qt::EditRole).toBool();
    EvaluationParametersItems::Auto sval = EvaluationParametersItems::fromIndex<EvaluationParametersItems::Auto>(idx);

    switch (sval) {
    case EvaluationParametersItems::Auto::FROM:
      ui->qle_from->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::TO:
      ui->qle_to->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::NOISE:
      ui->qle_noise->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_FROM_X:
      ui->qle_peakFromX->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_FROM_Y:
      ui->qle_peakFromY->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_TO_X:
      ui->qle_peakToX->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_TO_Y:
      ui->qle_peakToY->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::SLOPE_REF_POINT:
      ui->qle_slopeRefPoint->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::SLOPE_THRESHOLD:
      ui->qle_slopeThreshold->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::SLOPE_WINDOW:
      ui->qle_slopeWindow->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_X:
      ui->qle_peakX->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::PEAK_HEIGHT:
      ui->qle_peakHeight->setReadOnly(value);
    break;
    case EvaluationParametersItems::Auto::PEAK_WIDTH:
      ui->qle_peakWidthHalf->setReadOnly(value);
      break;
    default:
      break;
    }
  }
}

void EvaluationWidget::onFileComboBoxChanged(const int idx)
{
  emit fileSwitched(idx);
}

void EvaluationWidget::onFileSwitched(const int idx)
{
  ui->qcbox_files->setCurrentIndex(idx);
}

void EvaluationWidget::onFindPeaksClicked()
{
  emit findPeaks();
}

void EvaluationWidget::onReplotHvl()
{
  emit replotHvl();
}

void EvaluationWidget::onShowWindowComboBoxChanged(const int idx)
{
  QModelIndex midx = ui->qcbox_showWindow->model()->index(idx, 0);
  EvaluationParametersItems::ComboShowWindow val = ui->qcbox_showWindow->model()->data(midx, Qt::UserRole + 1).value<EvaluationParametersItems::ComboShowWindow>();
  emit comboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::SHOW_WINDOW,
                                                              EvaluationParametersItems::index(val)));
}

void EvaluationWidget::onWindowUnitsComboBoxChanged(const int idx)
{
  QModelIndex midx = ui->qcbox_windowUnits->model()->index(idx, 0);
  EvaluationParametersItems::ComboWindowUnits val = ui->qcbox_windowUnits->model()->data(midx, Qt::UserRole + 1).value<EvaluationParametersItems::ComboWindowUnits>();
  emit comboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::WINDOW_UNITS,
                                                              EvaluationParametersItems::index(val)));
}

void EvaluationWidget::setCommonParametersModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model)
{
  m_commonParametersWidget->setCommonParametersModel(model);
}

void EvaluationWidget::setDefaultState()
{
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::FINDER_PARAMETERS);
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::PEAK_PROPERTIES);
}

void EvaluationWidget::setEvaluationParametersAutoModel(AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model)
{
  m_evaluationParametersAutoMapper->setModel(model);

  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakFromX, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_X));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakFromY, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_Y));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeaktoX, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_X));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakToY, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_Y));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoFrom, model->indexFromItem(EvaluationParametersItems::Auto::FROM));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoTo, model->indexFromItem(EvaluationParametersItems::Auto::TO));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoNoise, model->indexFromItem(EvaluationParametersItems::Auto::NOISE));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeRefPoint, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_REF_POINT));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeThreshold, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_THRESHOLD));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeWindow, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakX, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_X));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakHeight, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_HEIGHT));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoPeakWidthHalf, model->indexFromItem(EvaluationParametersItems::Auto::PEAK_WIDTH));
  m_evaluationParametersAutoMapper->toFirst();

  connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(onEvaluationAutoModelChanged(QModelIndex, QModelIndex, QVector<int>)));
}

void EvaluationWidget::setEvaluationBaselineModel(QAbstractItemModel* model)
{
  ui->qcbox_baselineAlgorithm->setModel(model);
}

void EvaluationWidget::setEvaluationHvlFixedModel(AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *model)
{
  m_evaluationHvlFitFixedMapper->setModel(model);

  m_evaluationHvlFitFixedMapper->addMapping(ui->qcb_hvlA0Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_A0));
  m_evaluationHvlFitFixedMapper->addMapping(ui->qcb_hvlA1Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_A1));
  m_evaluationHvlFitFixedMapper->addMapping(ui->qcb_hvlA2Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_A2));
  m_evaluationHvlFitFixedMapper->addMapping(ui->qcb_hvlA3Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_A3));
  m_evaluationHvlFitFixedMapper->toFirst();
}

void EvaluationWidget::setEvaluationHvlFitIntModel(AbstractMapperModel<int, HVLFitParametersItems::Int> *model)
{
  m_evaluationHvlFitIntMapper->setModel(model);

  m_evaluationHvlFitIntMapper->addMapping(ui->qle_hvlDigits, model->indexFromItem(HVLFitParametersItems::Int::DIGITS));
  m_evaluationHvlFitIntMapper->addMapping(ui->qle_hvlMaxIterations, model->indexFromItem(HVLFitParametersItems::Int::ITERATIONS));
  m_evaluationHvlFitIntMapper->toFirst();
}

void EvaluationWidget::setEvaluationHvlFitModel(AbstractMapperModel<double, HVLFitResultsItems::Floating> *model)
{
  m_evaluationHvlFitMapper->setModel(model);
  m_evaluationHvlFitMapper->setItemDelegate(&m_floatingMapperDelegate);

  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA0, model->indexFromItem(HVLFitResultsItems::Floating::HVL_A0));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA1, model->indexFromItem(HVLFitResultsItems::Floating::HVL_A1));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA2, model->indexFromItem(HVLFitResultsItems::Floating::HVL_A2));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA3, model->indexFromItem(HVLFitResultsItems::Floating::HVL_A3));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlChiSquared, model->indexFromItem(HVLFitResultsItems::Floating::HVL_CHI_SQUARED));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlEpsilon, model->indexFromItem(HVLFitResultsItems::Floating::HVL_EPSILON));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlTUSP, model->indexFromItem(HVLFitResultsItems::Floating::HVL_TUSP));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA1Mobility, model->indexFromItem(HVLFitResultsItems::Floating::HVL_U_EFF_A1));
  m_evaluationHvlFitMapper->toFirst();
}

void EvaluationWidget::setEvaluationLoadedFilesModel(QAbstractItemModel *model)
{
  ui->qcbox_files->setModel(model);
}

void EvaluationWidget::setEvaluationParametersBooleanModel(AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *model)
{
  m_evaluationParametersBooleanMapper->setModel(model);

  m_evaluationParametersBooleanMapper->addMapping(ui->qcb_noiseSlopeCorrection, model->indexFromItem(EvaluationParametersItems::Boolean::NOISE_CORRECTION));
  m_evaluationParametersBooleanMapper->toFirst();
}

void EvaluationWidget::setEvaluationParametersFloatingModel(AbstractMapperModel<double, EvaluationParametersItems::Floating> *model)
{
  m_evaluationParametersFloatingMapper->setModel(model);
  m_evaluationParametersFloatingMapper->setItemDelegate(&m_floatingMapperDelegate);

  m_evaluationParametersFloatingMapper->addMapping(ui->qle_noiseWindow, model->indexFromItem(EvaluationParametersItems::Floating::NOISE_WINDOW));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakWindow, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_WINDOW));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_slopeWindow, model->indexFromItem(EvaluationParametersItems::Floating::SLOPE_WINDOW));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_noise, model->indexFromItem(EvaluationParametersItems::Floating::NOISE));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_noiseLODcoeff, model->indexFromItem(EvaluationParametersItems::Floating::NOISE_LOD_COEFF));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_slopeSensitivity, model->indexFromItem(EvaluationParametersItems::Floating::SLOPE_SENSITIVITY));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_slopeThreshold, model->indexFromItem(EvaluationParametersItems::Floating::SLOPE_THRESHOLD));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_from, model->indexFromItem(EvaluationParametersItems::Floating::FROM));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_to, model->indexFromItem(EvaluationParametersItems::Floating::TO));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_noiseRefPoint, model->indexFromItem(EvaluationParametersItems::Floating::NOISE_REF_POINT));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_slopeRefPoint, model->indexFromItem(EvaluationParametersItems::Floating::SLOPE_REF_POINT));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakFromX, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_X));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakFromY, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_Y));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakToX, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_X));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakToY, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_Y));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakX, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_X));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakHeight, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_HEIGHT));
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_peakWidthHalf, model->indexFromItem(EvaluationParametersItems::Floating::PEAK_WIDTH));
  m_evaluationParametersFloatingMapper->toFirst();
}

void EvaluationWidget::setEvaluationResultsModel(AbstractMapperModel<double, EvaluationResultsItems::Floating> *model)
{
  m_evaluationResultsMapper->setModel(model);
  m_evaluationResultsMapper->setItemDelegate(&m_floatingMapperDelegate);

  m_evaluationResultsMapper->addMapping(ui->qle_vEof, model->indexFromItem(EvaluationResultsItems::Floating::EOF_VELOCITY));
  m_evaluationResultsMapper->addMapping(ui->qle_vEofEff, model->indexFromItem(EvaluationResultsItems::Floating::EOF_VELOCITY_EFF));
  m_evaluationResultsMapper->addMapping(ui->qle_uEof, model->indexFromItem(EvaluationResultsItems::Floating::EOF_MOBILITY));
  m_evaluationResultsMapper->addMapping(ui->qle_peakFromXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_FROM_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakFromYRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  m_evaluationResultsMapper->addMapping(ui->qle_peakToXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_TO_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakToYRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_TO_Y));
  m_evaluationResultsMapper->addMapping(ui->qle_peakXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakHeightRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_HEIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_vPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  m_evaluationResultsMapper->addMapping(ui->qle_vPeakEff, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  m_evaluationResultsMapper->addMapping(ui->qle_uPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  m_evaluationResultsMapper->addMapping(ui->qle_peakHeightBLRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  m_evaluationResultsMapper->addMapping(ui->qle_areaPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_AREA));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfLeft, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfRight, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfFull, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfMLeft, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MET_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfMRight, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfMFull, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MET_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaLeft, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MIN_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaRight, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MIN_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaFull, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MIN_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaMLeft, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MET_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaMRight, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MET_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakSigmaMFull, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MET_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNLeft, model->indexFromItem(EvaluationResultsItems::Floating::N_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNRight, model->indexFromItem(EvaluationResultsItems::Floating::N_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNFull, model->indexFromItem(EvaluationResultsItems::Floating::N_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNhLeft, model->indexFromItem(EvaluationResultsItems::Floating::N_H_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNhRight, model->indexFromItem(EvaluationResultsItems::Floating::N_H_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNhFull, model->indexFromItem(EvaluationResultsItems::Floating::N_H_FULL));
  m_evaluationResultsMapper->toFirst();
}

void EvaluationWidget::setEvaluationShowWindowModel(QAbstractItemModel *model)
{
  ui->qcbox_showWindow->setModel(model);
}

void EvaluationWidget::setEvaluationWindowUnitsModel(QAbstractItemModel *model)
{
  ui->qcbox_windowUnits->setModel(model);
}
