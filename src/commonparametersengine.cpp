#include "commonparametersengine.h"
#include "commonparametersitems.h"

CommonParametersEngine::Context::Context(const MappedVectorWrapper<double, CommonParametersItems::Floating> &numData,
                                         const MappedVectorWrapper<bool, CommonParametersItems::Boolean> &boolData) :
  numData(numData),
  boolData(boolData),
  m_valid(true)
{
}

CommonParametersEngine::Context::Context() :
  m_valid(false)
{
}

void CommonParametersEngine::checkValidity() const
{
  const double capillary = m_numData.at(CommonParametersItems::Floating::CAPILLARY);
  const double detector = m_numData.at(CommonParametersItems::Floating::DETECTOR);
  const bool noEof = m_boolData.at(CommonParametersItems::Boolean::NO_EOF);

  emit validityState(capillary > 0 && capillary > detector, CommonParametersItems::Floating::CAPILLARY);
  emit validityState(detector > 0, CommonParametersItems::Floating::DETECTOR);
  emit validityState(m_numData.at(CommonParametersItems::Floating::VOLTAGE) != 0, CommonParametersItems::Floating::VOLTAGE);
  emit validityState(m_numData.at(CommonParametersItems::Floating::T_EOF) > 0 || noEof, CommonParametersItems::Floating::T_EOF);
  emit validityState(m_numData.at(CommonParametersItems::Floating::SELECTOR) >= 0, CommonParametersItems::Floating::SELECTOR);
}

bool CommonParametersEngine::Context::isValid() const
{
  return m_valid;
}

CommonParametersEngine::CommonParametersEngine(QObject *parent) :
  QObject(parent)
{
  connect(&m_numModel, &FloatingMapperModel<CommonParametersItems::Floating>::dataChanged, this, &CommonParametersEngine::onNumModelDataChanged);
  connect(&m_boolModel, &BooleanMapperModel<CommonParametersItems::Boolean>::dataChanged, this, &CommonParametersEngine::onNoEofStateChanged);

  m_numModel.setUnderlyingData(m_numData.pointer());
  m_boolModel.setUnderlyingData(m_boolData.pointer());
}

bool CommonParametersEngine::boolValue(const CommonParametersItems::Boolean item) const
{
  return m_boolData.at(item);
}

AbstractMapperModel<bool, CommonParametersItems::Boolean> *CommonParametersEngine::boolModel()
{
  return &m_boolModel;
}

CommonParametersEngine::Context CommonParametersEngine::currentContext() const
{
  return CommonParametersEngine::Context(m_numData, m_boolData);
}

AbstractMapperModel<double, CommonParametersItems::Floating> *CommonParametersEngine::numModel()
{
  return &m_numModel;
}

double CommonParametersEngine::numValue(const CommonParametersItems::Floating item) const
{
  return m_numData.at(item);
}

void CommonParametersEngine::onNumModelDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  if (!roles.contains(Qt::EditRole))
    return;

  bool emitParams = false;

  for (int idx = topLeft.column(); idx <= bottomRight.column(); idx++) {

    CommonParametersItems::Floating sval = m_numModel.itemFromIndex(idx);

    /* Act only on values that affect the electric field strength */
    switch (sval) {
    case CommonParametersItems::Floating::CAPILLARY:
    case CommonParametersItems::Floating::VOLTAGE:
      emitParams = recalculate();
      checkValidity();
      break;
    case CommonParametersItems::Floating::DETECTOR:
    case CommonParametersItems::Floating::T_EOF:
      checkValidity();
      emitParams = true;
      break;
    default:
      break;
    }
  }

  if (emitParams)
    emit parametersUpdated();
}

void CommonParametersEngine::onNoEofStateChanged()
{
  emit noEofStateChanged(m_boolData.at(CommonParametersItems::Boolean::NO_EOF));
}

void CommonParametersEngine::onUpdateTEof(const double t)
{
  m_numData[CommonParametersItems::Floating::T_EOF] = t;

  m_numModel.notifyDataChanged(CommonParametersItems::Floating::T_EOF, CommonParametersItems::Floating::T_EOF, { Qt::EditRole });
}

bool CommonParametersEngine::recalculate()
{
  const double lengthDetectorMeters = m_numData.at(CommonParametersItems::Floating::DETECTOR) / 100;
  const double lengthMeters = m_numData.at(CommonParametersItems::Floating::CAPILLARY) / 100.0;
  const double voltage = m_numData.at(CommonParametersItems::Floating::VOLTAGE);

  if (lengthMeters > 0.0 &&
      (lengthMeters > lengthDetectorMeters)) {
    double fieldStrength = voltage / lengthMeters;

    if (fieldStrength != m_numData.at(CommonParametersItems::Floating::FIELD)) {
      m_numModel.setData(m_numModel.index(0, m_numModel.indexFromItem(CommonParametersItems::Floating::FIELD)), fieldStrength, Qt::EditRole);
      return true;
    }
  }

  return false;
}

void CommonParametersEngine::revalidate() const
{
  checkValidity();
}

bool CommonParametersEngine::setContext(const Context &ctx)
{
  if (!ctx.isValid())
    return false;

  m_numData = ctx.numData;
  m_boolData = ctx.boolData;

  m_numModel.notifyAllDataChanged({ Qt::EditRole });
  m_boolModel.notifyAllDataChanged();

  return true;
}

