#include "dataexporter.h"
#include "evaluationengine.h"

#define REGISTER_EXPORTABLE(name, getter) \
  m_dataExporter->registerExportable(name, \
    [](EvaluationEngine *me) { \
      if (!me->isContextValid()) \
        return QVariant(); \
      return QVariant(me->getter); \
  })

void EvaluationEngine::initDataExporter()
{
  REGISTER_EXPORTABLE("Mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
  REGISTER_EXPORTABLE("Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
}
