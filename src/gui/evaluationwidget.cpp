#include "evaluationwidget.h"
#include "ui_evaluationwidget.h"
#include "../evaluationwidgetconnector.h"

EvaluationWidget::EvaluationWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::EvaluationWidget)
{
  m_evaluationParametersAutoMapper = new QDataWidgetMapper(this);
  m_evaluationParametersBooleanMapper = new QDataWidgetMapper(this);
  m_evaluationParametersFloatingMapper = new QDataWidgetMapper(this);
  m_evaluationResultsMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitBooleanMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitIntMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitMapper = new QDataWidgetMapper(this);
  m_evaluationHvlFitOptionsMapper = new QDataWidgetMapper(this);
  m_evaluationHvlExtrapolationBooleanMapper = new QDataWidgetMapper(this);
  m_evaluationHvlExtrapolationFloatingMapper = new QDataWidgetMapper(this);
  m_snrMapper = new QDataWidgetMapper(this);

  m_evaluationResultsMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

  ui->setupUi(this);

  ui->qle_hvlDigits->setDisabled(ui->qcb_hvlAutoDigits->checkState() == Qt::Checked);

#ifdef CEVAL_PUBLIC_RELEASE
  ui->qtabw_evaluation->removeTab(2); /* Disable hackish export functionality by hiding its controls */
#endif // CEVAL_PUBLIC_RELEASE

  ui->qpb_findPeak->setVisible(false);
  ui->qtabw_evaluation->removeTab(0);

  connect(ui->qpb_defaultFinderParameters, &QPushButton::clicked, this, &EvaluationWidget::onDefaultFinderParametersClicked);
  connect(ui->qcbox_baselineAlgorithm, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onBaselineComboBoxChanged);
  connect(ui->qcbox_showWindow, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onShowWindowComboBoxChanged);
  connect(ui->qcbox_windowUnits, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onWindowUnitsComboBoxChanged);
  connect(ui->qpb_findPeak, &QPushButton::clicked, this, &EvaluationWidget::onFindPeaksClicked);
  connect(ui->qpb_doHvlFit, &QPushButton::clicked, this, &EvaluationWidget::onDoHvlFitClicked);
  connect(ui->qpb_replotHvl, &QPushButton::clicked, this, &EvaluationWidget::onReplotHvl);
  connect(ui->qpb_ctcEOF, &QPushButton::clicked, this, &EvaluationWidget::onCtcEOFClicked);
  connect(ui->qpb_ctcHVL, &QPushButton::clicked, this, &EvaluationWidget::onCtcHVLClicked);
  connect(ui->qpb_ctcPeak, &QPushButton::clicked, this, &EvaluationWidget::onCtcPeakClicked);
  connect(ui->qpb_ctcPeakDims, &QPushButton::clicked, this, &EvaluationWidget::onCtcPeakDimsClicked);
  connect(ui->qpb_configureExporterBackend, &QPushButton::clicked, this, &EvaluationWidget::onConfigureExporterBackendClicked);
  connect(ui->qpb_manageSchemes, &QPushButton::clicked, this, &EvaluationWidget::onManageExporterSchemesClicked);
  connect(ui->qpb_export, &QPushButton::clicked, this, &EvaluationWidget::onExportSchemeClicked);
  connect(ui->qcbox_availableExporterBackends, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationWidget::onExporterBackendComboBoxChanged);
  connect(ui->qcbox_schemes, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EvaluationWidget::onExporterSchemeComboBoxChanged);
  connect(ui->qcbox_clipboardDataArrangement, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EvaluationWidget::onClipboardExporterArrangementChanged);
  connect(ui->qle_clipboardDelimiter, &QLineEdit::textChanged, this, &EvaluationWidget::onClipboardExporterDelimiterTextChanged);
  connect(ui->qcb_exportOnLeave, &QCheckBox::clicked, this, &EvaluationWidget::onExportOnFileLeftClicked);
  connect(ui->qcb_hvlAutoDigits, &QCheckBox::stateChanged, this, &EvaluationWidget::onHvlAutoDigitsStateChanged);
}

EvaluationWidget::~EvaluationWidget()
{
  delete ui;
}

void EvaluationWidget::connectToAccumulator(QObject *dac)
{
  EvaluationWidgetConnector::connectAll(this, dac);
}

void EvaluationWidget::onBaselineComboBoxChanged(const int idx){
  QModelIndex midx = ui->qcbox_baselineAlgorithm->model()->index(idx, 0);
  EvaluationParametersItems::ComboBaselineAlgorithm val = ui->qcbox_baselineAlgorithm->model()->data(midx, Qt::UserRole + 1).value<EvaluationParametersItems::ComboBaselineAlgorithm>();
  emit comboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                              EvaluationParametersItems::index(val)));
}

void EvaluationWidget::onClipboardExporterArrangementChanged(const int idx)
{
  const QModelIndex &midx = ui->qcbox_clipboardDataArrangement->model()->index(idx, 0);

  if (!midx.isValid())
    return;

  emit clipboardExporterArrangementChanged(midx);
}

void EvaluationWidget::onClipboardExporterDelimiterTextChanged(const QString &text)
{
  emit clipboardExporterDelimiterChanged(text);
}

void EvaluationWidget::onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier)
{
  switch (notifier.id) {
  case EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM:
    ui->qcbox_baselineAlgorithm->setCurrentIndex(notifier.value);
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

void EvaluationWidget::onClipboardExporterDataArrangementSet(const QModelIndex &midx)
{
  if (!midx.isValid())
    return;

  disconnect(ui->qcbox_clipboardDataArrangement, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EvaluationWidget::onClipboardExporterArrangementChanged);

  ui->qcbox_clipboardDataArrangement->setCurrentIndex(midx.row());

  connect(ui->qcbox_clipboardDataArrangement, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EvaluationWidget::onClipboardExporterArrangementChanged);
}

void EvaluationWidget::onClipboardExporterDelimiterSet(const QString &text)
{
  disconnect(ui->qle_clipboardDelimiter, &QLineEdit::textChanged, this, &EvaluationWidget::onClipboardExporterDelimiterTextChanged);

  ui->qle_clipboardDelimiter->setText(text);

  connect(ui->qle_clipboardDelimiter, &QLineEdit::textChanged, this, &EvaluationWidget::onClipboardExporterDelimiterTextChanged);
}

void EvaluationWidget::onConfigureExporterBackendClicked()
{
  emit configureExporterBackend();
}

void EvaluationWidget::onCtcEOFClicked()
{
  emit copyToClipboard(EvaluationEngineMsgs::CopyToClipboard::EOFLOW);
}

void EvaluationWidget::onCtcHVLClicked()
{
  emit copyToClipboard(EvaluationEngineMsgs::CopyToClipboard::HVL);
}

void EvaluationWidget::onCtcPeakClicked()
{
  emit copyToClipboard(EvaluationEngineMsgs::CopyToClipboard::PEAK);
}

void EvaluationWidget::onCtcPeakDimsClicked()
{
  emit copyToClipboard(EvaluationEngineMsgs::CopyToClipboard::PEAK_DIMS);
}

void EvaluationWidget::onDefaultFinderParametersClicked()
{
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::FINDER_PARAMETERS);
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
    case EvaluationParametersItems::Auto::SLOPE_REF_POINT:
      ui->qle_slopeRefPoint->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::SLOPE_THRESHOLD:
      ui->qle_slopeThreshold->setReadOnly(value);
      break;
    case EvaluationParametersItems::Auto::SLOPE_WINDOW:
      ui->qle_slopeWindow->setReadOnly(value);
      break;
    default:
      break;
    }
  }
}

void EvaluationWidget::onExporterBackendSet(const QModelIndex &idx)
{
  ui->qcbox_availableExporterBackends->setCurrentIndex(idx.row());
}

void EvaluationWidget::onExportSchemeClicked()
{
  emit exportScheme();
}

void EvaluationWidget::onExportOnFileLeftClicked()
{
  emit exportOnFileLeftToggled(ui->qcb_exportOnLeave->checkState() == Qt::Checked);
}

void EvaluationWidget::onFindPeaksClicked()
{
  emit findPeaks();
}

void EvaluationWidget::onHvlAutoDigitsStateChanged()
{
  ui->qle_hvlDigits->setDisabled(ui->qcb_hvlAutoDigits->checkState() == Qt::Checked);
}

void EvaluationWidget::onManageExporterSchemesClicked()
{
  emit manageExporterSchemes();
}

void EvaluationWidget::onReplotHvl()
{
  emit replotHvl();
}

void EvaluationWidget::onExporterBackendComboBoxChanged(const int idx)
{
  const QModelIndex &midx = ui->qcbox_availableExporterBackends->model()->index(idx, 0);
  if (!midx.isValid())
    return;

  emit exporterBackendChanged(midx);
}

void EvaluationWidget::onExporterSchemeComboBoxChanged(const int idx)
{
  const QModelIndex &midx = ui->qcbox_schemes->model()->index(idx, 0);
  if (!midx.isValid())
    return;

  emit exporterSchemeChanged(midx);
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

void EvaluationWidget::setClipboardDataArrangementModel(QAbstractItemModel *model)
{
  ui->qcbox_clipboardDataArrangement->setModel(model);
}

void EvaluationWidget::setDefaultState()
{
  emit evaluationSetDefault(EvaluationEngineMsgs::Default::FINDER_PARAMETERS);

  on_qcb_enableHvlExtrapolation_clicked(ui->qcb_enableHvlExtrapolation->isChecked());
}

void EvaluationWidget::setEvaluationParametersAutoModel(AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model)
{
  m_evaluationParametersAutoMapper->setModel(model);

  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoFrom, model->indexFromItem(EvaluationParametersItems::Auto::FROM));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoTo, model->indexFromItem(EvaluationParametersItems::Auto::TO));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoNoise, model->indexFromItem(EvaluationParametersItems::Auto::NOISE));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeRefPoint, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_REF_POINT));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeThreshold, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_THRESHOLD));
  m_evaluationParametersAutoMapper->addMapping(ui->qcb_autoSlopeWindow, model->indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW));
  m_evaluationParametersAutoMapper->toFirst();

  connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(onEvaluationAutoModelChanged(QModelIndex, QModelIndex, QVector<int>)));
}

void EvaluationWidget::setEvaluationBaselineModel(QAbstractItemModel* model)
{
  ui->qcbox_baselineAlgorithm->setModel(model);
}

void EvaluationWidget::setEvaluationHvlBooleanModel(AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *model)
{
  m_evaluationHvlFitBooleanMapper->setModel(model);

  m_evaluationHvlFitBooleanMapper->addMapping(ui->qcb_hvlA0Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_FIX_A0));
  m_evaluationHvlFitBooleanMapper->addMapping(ui->qcb_hvlA1Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_FIX_A1));
  m_evaluationHvlFitBooleanMapper->addMapping(ui->qcb_hvlA2Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_FIX_A2));
  m_evaluationHvlFitBooleanMapper->addMapping(ui->qcb_hvlA3Fixed, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_FIX_A3));
  m_evaluationHvlFitBooleanMapper->addMapping(ui->qcb_hvlAutoDigits, model->indexFromItem(HVLFitParametersItems::Boolean::HVL_AUTO_DIGITS));
  m_evaluationHvlFitBooleanMapper->toFirst();
}

void EvaluationWidget::setEvaluationHvlExtrapolationModels(std::tuple<AbstractMapperModel<bool, HVLExtrapolationParametersItems::Boolean> *, AbstractMapperModel<double, HVLExtrapolationParametersItems::Floating> *> models)
{
  AbstractMapperModel<bool, HVLExtrapolationParametersItems::Boolean> *m_boolean = std::get<0>(models);

  m_evaluationHvlExtrapolationBooleanMapper->setModel(m_boolean);
  m_evaluationHvlExtrapolationBooleanMapper->addMapping(ui->qcb_enableHvlExtrapolation, m_boolean->indexFromItem(HVLExtrapolationParametersItems::Boolean::ENABLE));
  m_evaluationHvlExtrapolationBooleanMapper->toFirst();

  AbstractMapperModel<double, HVLExtrapolationParametersItems::Floating> *m_floating = std::get<1>(models);

  m_evaluationHvlExtrapolationFloatingMapper->setModel(m_floating);
  m_evaluationHvlExtrapolationFloatingMapper->addMapping(ui->qle_hvlExtrBaselineCloseness, m_floating->indexFromItem(HVLExtrapolationParametersItems::Floating::TOLERANCE));
  m_evaluationHvlExtrapolationFloatingMapper->toFirst();
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
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlChiSquared, model->indexFromItem(HVLFitResultsItems::Floating::HVL_S));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlEpsilon, model->indexFromItem(HVLFitResultsItems::Floating::HVL_EPSILON));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlTUSP, model->indexFromItem(HVLFitResultsItems::Floating::HVL_TUSP));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlA1Mobility, model->indexFromItem(HVLFitResultsItems::Floating::HVL_U_EFF_A1));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlExtrVarianceMean, model->indexFromItem(HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_VARIANCE));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlExtrSigmaMean, model->indexFromItem(HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_SIGMA));
  m_evaluationHvlFitMapper->addMapping(ui->qle_hvlExtrMeanAtX, model->indexFromItem(HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_MEAN));
  m_evaluationHvlFitMapper->toFirst();
}

void EvaluationWidget::setEvaluationHvlFitOptionsModel(AbstractMapperModel<bool, HVLFitOptionsItems::Boolean> *model)
{
  m_evaluationHvlFitOptionsMapper->setModel(model);

  m_evaluationHvlFitOptionsMapper->addMapping(ui->qcb_disableAutoFit, model->indexFromItem(HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT));
  m_evaluationHvlFitOptionsMapper->addMapping(ui->qcb_showHvlStats, model->indexFromItem(HVLFitOptionsItems::Boolean::SHOW_FIT_STATS));
  m_evaluationHvlFitOptionsMapper->toFirst();
}

void EvaluationWidget::setEvaluationParametersBooleanModel(AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *model)
{
  m_evaluationParametersBooleanMapper->setModel(model);

  m_evaluationParametersBooleanMapper->addMapping(ui->qcb_noiseSlopeCorrection, model->indexFromItem(EvaluationParametersItems::Boolean::NOISE_CORRECTION));
  m_evaluationParametersBooleanMapper->addMapping(ui->qcb_disturbanceDetection, model->indexFromItem(EvaluationParametersItems::Boolean::DISTURBANCE_DETECTION));
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
  m_evaluationParametersFloatingMapper->addMapping(ui->qle_disturbanceDetection, model->indexFromItem(EvaluationParametersItems::Floating::DISTURBANCE_DETECTION));
  m_evaluationParametersFloatingMapper->toFirst();
}

void EvaluationWidget::setEvaluationResultsModel(AbstractMapperModel<double, EvaluationResultsItems::Floating> *model)
{
  m_evaluationResultsMapper->setModel(model);
  m_evaluationResultsMapper->setItemDelegate(&m_floatingMapperDelegate);

  m_evaluationResultsMapper->addMapping(ui->qle_vEof, model->indexFromItem(EvaluationResultsItems::Floating::EOF_VELOCITY));
  m_evaluationResultsMapper->addMapping(ui->qle_uEof, model->indexFromItem(EvaluationResultsItems::Floating::EOF_MOBILITY));
  m_evaluationResultsMapper->addMapping(ui->qle_peakFromXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_FROM_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakFromYRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  m_evaluationResultsMapper->addMapping(ui->qle_peakToXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_TO_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakToYRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_TO_Y));
  m_evaluationResultsMapper->addMapping(ui->qle_peakXRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakHeightRes, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  m_evaluationResultsMapper->addMapping(ui->qle_vPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  m_evaluationResultsMapper->addMapping(ui->qle_vPeakEff, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  m_evaluationResultsMapper->addMapping(ui->qle_uPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  m_evaluationResultsMapper->addMapping(ui->qle_areaPeak, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_AREA));
  m_evaluationResultsMapper->addMapping(ui->qle_correctedArea, model->indexFromItem(EvaluationResultsItems::Floating::PEAK_CORRECTED_AREA));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfLeft, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfRight, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfFull, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_FULL));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfMLeft, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MET_LEFT));
  m_evaluationResultsMapper->addMapping(ui->qle_peakWidthHalfMRight, model->indexFromItem(EvaluationResultsItems::Floating::WIDTH_HALF_MET_RIGHT));
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
  m_evaluationResultsMapper->addMapping(ui->qle_varianceApex, model->indexFromItem(EvaluationResultsItems::Floating::VARIANCE_APEX));
  m_evaluationResultsMapper->addMapping(ui->qle_varianceCentroid, model->indexFromItem(EvaluationResultsItems::Floating::VARIANCE_MEAN));
  m_evaluationResultsMapper->addMapping(ui->qle_sigmaApex, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_APEX));
  m_evaluationResultsMapper->addMapping(ui->qle_sigmaCentroid, model->indexFromItem(EvaluationResultsItems::Floating::SIGMA_MEAN));
  m_evaluationResultsMapper->addMapping(ui->qle_centroidX, model->indexFromItem(EvaluationResultsItems::Floating::MEAN_X));
  m_evaluationResultsMapper->addMapping(ui->qle_peakNAsym, model->indexFromItem(EvaluationResultsItems::Floating::N_ASYM));
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

void EvaluationWidget::setExporterBackendsModel(QAbstractItemModel *model)
{
  ui->qcbox_availableExporterBackends->setModel(model);
}

void EvaluationWidget::setExporterSchemesModel(QAbstractItemModel *model)
{
  ui->qcbox_schemes->setModel(model);
}

void EvaluationWidget::setSNRModel(AbstractMapperModel<double, SNRItems::Floating> *model)
{
  m_snrMapper->setModel(model);
  m_snrMapper->setItemDelegate(&m_floatingMapperDelegate);

  m_snrMapper->addMapping(ui->qle_SNRbaselineSigma, model->indexFromItem(SNRItems::Floating::BASELINE_SIGMA));
  m_snrMapper->addMapping(ui->qle_SNRsnr, model->indexFromItem(SNRItems::Floating::SIGNAL_TO_NOISE_RATIO));
  m_snrMapper->addMapping(ui->qle_SNRsigmaAmplifier, model->indexFromItem(SNRItems::Floating::INPUT_SIGMA_AMPLIFIER));
  m_snrMapper->toFirst();
}

void EvaluationWidget::on_qcb_enableHvlExtrapolation_clicked(bool checked)
{
  ui->qle_hvlExtrBaselineCloseness->setEnabled(checked);
  ui->qle_hvlExtrMeanAtX->setEnabled(checked);
  ui->qle_hvlExtrSigmaMean->setEnabled(checked);
  ui->qle_hvlExtrVarianceMean->setEnabled(checked);

  emit hvlExtrapolationToggled(checked);
}
