#include "dataexporter.h"
#include "evaluationengine.h"

#define MAKE_EXPORTABLE(map, name, getter) \
  map.insert(name, new DataExporter<EvaluationEngine>::Exportable(name, \
    [](const EvaluationEngine *me) { \
      if (!me->isContextValid()) \
        return QVariant(); \
      return QVariant(me->getter); \
  }))

void EvaluationEngine::initDataExporter()
{
  DataExporter<EvaluationEngine>::ExportablesMap exportables;

  try {
    MAKE_EXPORTABLE(exportables, "EOF velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
    MAKE_EXPORTABLE(exportables, "Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
    MAKE_EXPORTABLE(exportables, "Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
    MAKE_EXPORTABLE(exportables, "Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
    MAKE_EXPORTABLE(exportables, "Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
    MAKE_EXPORTABLE(exportables, "Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
    MAKE_EXPORTABLE(exportables, "Peak velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
    MAKE_EXPORTABLE(exportables, "Peak effective velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
    MAKE_EXPORTABLE(exportables, "Peak mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
    MAKE_EXPORTABLE(exportables, "Peak effective mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
    MAKE_EXPORTABLE(exportables, "Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
    MAKE_EXPORTABLE(exportables, "Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  } catch (std::bad_alloc &) {
    /* TODO: Do something useful here */
  }


}
