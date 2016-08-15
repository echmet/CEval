#include "dataexporter.h"
#include "evaluationengine.h"

#define REGISTER_EXPORTABLE(name, getter) \
  m_dataExporter->registerExportable(name, \
    [](const EvaluationEngine *me) { \
      if (!me->isContextValid()) \
        return QVariant(); \
      return QVariant(me->getter); \
  })

void EvaluationEngine::initDataExporter()
{
  REGISTER_EXPORTABLE("EOF velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
  REGISTER_EXPORTABLE("Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
  REGISTER_EXPORTABLE("Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  REGISTER_EXPORTABLE("Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
  REGISTER_EXPORTABLE("Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
  REGISTER_EXPORTABLE("Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
  REGISTER_EXPORTABLE("Peak velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  REGISTER_EXPORTABLE("Peak effective velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  REGISTER_EXPORTABLE("Peak mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
  REGISTER_EXPORTABLE("Peak effective mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  REGISTER_EXPORTABLE("Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  REGISTER_EXPORTABLE("Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
}
