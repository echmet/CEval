#include "commonparametersengine.h"
#include "commonparametersitems.h"

CommonParametersEngine::Context::Context(const MappedVectorWrapper<double, CommonParametersItems::Floating> &data) :
  data(data),
  m_valid(true)
{
}

CommonParametersEngine::Context::Context() :
  m_valid(false)
{
}

void CommonParametersEngine::checkValidity() const
{
  emit validityState(m_data.at(CommonParametersItems::Floating::CAPILLARY) > 0, CommonParametersItems::Floating::CAPILLARY);
  emit validityState(m_data.at(CommonParametersItems::Floating::DETECTOR) > 0, CommonParametersItems::Floating::DETECTOR);
  emit validityState(m_data.at(CommonParametersItems::Floating::VOLTAGE) != 0, CommonParametersItems::Floating::VOLTAGE);
  emit validityState(m_data.at(CommonParametersItems::Floating::T_EOF) > 0, CommonParametersItems::Floating::T_EOF);
  emit validityState(m_data.at(CommonParametersItems::Floating::SELECTOR) >= 0, CommonParametersItems::Floating::SELECTOR);
}

bool CommonParametersEngine::Context::isValid() const
{
  return m_valid;
}

CommonParametersEngine::CommonParametersEngine(QObject *parent) :
  QObject(parent)
{
  connect(&m_model, &FloatingMapperModel<CommonParametersItems::Floating>::dataChanged, this, &CommonParametersEngine::onModelDataChanged);

  m_model.setUnderlyingData(m_data.pointer());
}

CommonParametersEngine::Context CommonParametersEngine::currentContext() const
{
  return CommonParametersEngine::Context(m_data);
}

AbstractMapperModel<double, CommonParametersItems::Floating> *CommonParametersEngine::model()
{
  return &m_model;
}

void CommonParametersEngine::onModelDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  if (!roles.contains(Qt::EditRole))
    return;

  bool recalcd = false;
  for (int idx = topLeft.column(); idx <= bottomRight.column(); idx++) {

    CommonParametersItems::Floating sval = m_model.itemFromIndex(idx);

    /* Act only on values that affect the electric field strength */
    switch (sval) {
    case CommonParametersItems::Floating::CAPILLARY:
    case CommonParametersItems::Floating::DETECTOR:
    case CommonParametersItems::Floating::VOLTAGE:
      if (!recalcd) {
        recalculate();
        recalcd = true;
        checkValidity();
      }
      break;
    case CommonParametersItems::Floating::T_EOF:
      checkValidity();
      emit tEofUpdated();
      break;
    default:
      break;
    }
  }
}

void CommonParametersEngine::onUpdateTEof(const double t)
{
  m_data[CommonParametersItems::Floating::T_EOF] = t;

  emit m_model.dataChanged(m_model.index(0, m_model.indexFromItem(CommonParametersItems::Floating::T_EOF)),
                           m_model.index(0, m_model.indexFromItem(CommonParametersItems::Floating::T_EOF)),
                           { Qt::EditRole });
}

void CommonParametersEngine::recalculate()
{
  double lengthMeters = m_data.at(CommonParametersItems::Floating::CAPILLARY) / 100.0;
  double voltage = m_data.at(CommonParametersItems::Floating::VOLTAGE);

  if (lengthMeters > 0.0) {
    double fieldStrength = voltage / lengthMeters;
    m_model.setData(m_model.index(0, m_model.indexFromItem(CommonParametersItems::Floating::FIELD)), fieldStrength, Qt::EditRole);
  }
}

void CommonParametersEngine::revalidate() const
{
  checkValidity();
}

bool CommonParametersEngine::setContext(const Context &ctx)
{
  if (!ctx.isValid())
    return false;

  m_data = ctx.data;

  emit m_model.dataChanged(m_model.index(0, 0), m_model.index(0, m_model.indexFromItem(CommonParametersItems::Floating::LAST_INDEX) - 1),
                           { Qt::EditRole });

  return true;
}

double CommonParametersEngine::value(const CommonParametersItems::Floating item) const
{
  return m_data.at(item);
}
