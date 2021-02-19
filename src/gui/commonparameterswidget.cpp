#include "commonparameterswidget.h"
#include "ui_commonparameterswidget.h"
#include "../commonparametersitems.h"
#include "../commonparameterswidgetconnector.h"
#include "../evaluationenginecommonparametersview.h"
#include "parameterscarryoverdialog.h"
#include <cassert>

CommonParametersWidget::CommonParametersWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CommonParametersWidget)
{
  m_commonParametersBoolMapper = new QDataWidgetMapper(this);
  m_commonParametersNumMapper = new QDataWidgetMapper(this);

  ui->setupUi(this);

  m_invalidPalette = this->palette();
  m_invalidPalette.setColor(QPalette::WindowText, Qt::red);

  ui->qcbox_eofSource->addItem(tr("EOF from tMax"),
                                  QVariant::fromValue<CommonParametersItems::EOFSource>(CommonParametersItems::EOFSource::MAXIMUM));
  ui->qcbox_eofSource->addItem(tr("EOF from HVL a1"),
                                  QVariant::fromValue<CommonParametersItems::EOFSource>(CommonParametersItems::EOFSource::HVL_A1));

  connect(ui->qpb_readEof, &QPushButton::clicked, this, &CommonParametersWidget::onReadEofClicked);
  connect(ui->qcb_noEof, &QMappedCheckBox::stateChanged, this, &CommonParametersWidget::onNoEofClicked);

  connect(ui->qcbox_eofSource, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &CommonParametersWidget::onEofSourceChanged);

  connect(
    ui->qpb_carryOverParamsBetweenData,
    &QPushButton::clicked,
    [this]() {
      ParametersCarryOverDialog dlg(m_eeView, this);
      dlg.exec();
    });
}

CommonParametersWidget::~CommonParametersWidget()
{
  delete ui;
}

void CommonParametersWidget::connectToAccumulator(QObject *dac)
{
  CommonParametersWidgetConnector::connectAll(this, dac);

  onEofSourceChanged();
}

void CommonParametersWidget::markAsInvalid(QWidget *w, const bool invalid)
{
  if (invalid)
    w->setPalette(m_invalidPalette);
  else
    w->setPalette(QPalette());
}

void CommonParametersWidget::onEofSourceChanged()
{
  auto v = ui->qcbox_eofSource->currentData();
  assert(v.canConvert<CommonParametersItems::EOFSource>());

  emit eofSourceChanged(v.value<CommonParametersItems::EOFSource>());
}

void CommonParametersWidget::onNoEofClicked()
{
  switch (ui->qcb_noEof->checkState()) {
  case Qt::Checked:
    ui->qle_tEOF->setEnabled(false);
    break;
  default:
    ui->qle_tEOF->setEnabled(true);
    break;
  }
}

void CommonParametersWidget::onReadEofClicked()
{
  emit readEof();
}

void CommonParametersWidget::onValidityState(const bool state, const CommonParametersItems::Floating item)
{
  switch (item) {
  case CommonParametersItems::Floating::CAPILLARY:
    markAsInvalid(ui->ql_capillary, !state);
    break;
  case CommonParametersItems::Floating::DETECTOR:
    markAsInvalid(ui->ql_detector, !state);
    break;
  case CommonParametersItems::Floating::SELECTOR:
    markAsInvalid(ui->ql_selector, !state);
    break;
  case CommonParametersItems::Floating::VOLTAGE:
    markAsInvalid(ui->ql_voltage, !state);
    break;
  case CommonParametersItems::Floating::T_EOF:
    markAsInvalid(ui->ql_tEOF, !state);
    break;
  default:
    break;
  }
}

void CommonParametersWidget::setCommonParametersBoolModel(AbstractMapperModel<bool, CommonParametersItems::Boolean> *model)
{
  m_commonParametersBoolMapper->setModel(model);

  m_commonParametersBoolMapper->addMapping(ui->qcb_noEof, model->indexFromItem(CommonParametersItems::Boolean::NO_EOF));
  m_commonParametersBoolMapper->toFirst();
}

void CommonParametersWidget::setCommonParametersNumModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model)
{
  m_commonParametersNumMapper->setModel(model);
  m_commonParametersNumMapper->setItemDelegate(&m_commonParametersMapperDelegate);

  m_commonParametersNumMapper->addMapping(ui->qle_capillary, model->indexFromItem(CommonParametersItems::Floating::CAPILLARY));
  m_commonParametersNumMapper->addMapping(ui->qle_detector, model->indexFromItem(CommonParametersItems::Floating::DETECTOR));
  m_commonParametersNumMapper->addMapping(ui->qle_fieldStrength, model->indexFromItem(CommonParametersItems::Floating::FIELD));
  m_commonParametersNumMapper->addMapping(ui->qle_selector, model->indexFromItem(CommonParametersItems::Floating::SELECTOR));
  m_commonParametersNumMapper->addMapping(ui->qle_tEOF, model->indexFromItem(CommonParametersItems::Floating::T_EOF));
  m_commonParametersNumMapper->addMapping(ui->qle_voltage, model->indexFromItem(CommonParametersItems::Floating::VOLTAGE));
  m_commonParametersNumMapper->toFirst();
}

void CommonParametersWidget::setEvaluationEngineCommonParametersView(EvaluationEngineCommonParametersView *eeView)
{
  assert(eeView != nullptr);

  m_eeView = eeView;
}
