#include "commonparameterswidget.h"
#include "ui_commonparameterswidget.h"
#include "../commonparametersitems.h"
#include "../commonparameterswidgetconnector.h"

CommonParametersWidget::CommonParametersWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CommonParametersWidget)
{
  m_commonParametersMapper = new QDataWidgetMapper(this);

  ui->setupUi(this);

  m_invalidPalette = this->palette();
  m_invalidPalette.setColor(QPalette::WindowText, Qt::red);

  connect(ui->qpb_readEof, &QPushButton::clicked, this, &CommonParametersWidget::onReadEofClicked);
}

CommonParametersWidget::~CommonParametersWidget()
{
  delete ui;
}

void CommonParametersWidget::connectToAccumulator(QObject *dac)
{
  CommonParametersWidgetConnector::connectAll(this, dac);
}

void CommonParametersWidget::markAsInvalid(QWidget *w, const bool invalid)
{
  if (invalid)
    w->setPalette(m_invalidPalette);
  else
    w->setPalette(QPalette());
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

void CommonParametersWidget::setCommonParametersModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model)
{
  m_commonParametersMapper->setModel(model);
  m_commonParametersMapper->setItemDelegate(&m_commonParametersMapperDelegate);

  m_commonParametersMapper->addMapping(ui->qle_capillary, model->indexFromItem(CommonParametersItems::Floating::CAPILLARY));
  m_commonParametersMapper->addMapping(ui->qle_detector, model->indexFromItem(CommonParametersItems::Floating::DETECTOR));
  m_commonParametersMapper->addMapping(ui->qle_fieldStrength, model->indexFromItem(CommonParametersItems::Floating::FIELD));
  m_commonParametersMapper->addMapping(ui->qle_selector, model->indexFromItem(CommonParametersItems::Floating::SELECTOR));
  m_commonParametersMapper->addMapping(ui->qle_tEOF, model->indexFromItem(CommonParametersItems::Floating::T_EOF));
  m_commonParametersMapper->addMapping(ui->qle_voltage, model->indexFromItem(CommonParametersItems::Floating::VOLTAGE));
  m_commonParametersMapper->toFirst();
}
