#include "dataexporter.h"
#include "evaluationengine.h"

#define MAKE_EXPORTABLE(map, TT, name, getter) \
  map.insert(name, new Exportable<TT>(name, \
    [](const TT *me) { \
      if (!me->isContextValid()) \
        return QVariant(); \
      return QVariant(me->getter); \
  }))


void EvaluationEngine::initDataExporter()
{
  DataExporter::ExportablesMap exportables;
  DataExporter::ExportablesMap listExportables;

  try {
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "EOF velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak effective velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak effective mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
    MAKE_EXPORTABLE(exportables, EvaluationEngine, "Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  } catch (std::bad_alloc &) {
    /* TODO: Do something useful here */
  }

  DataExporter::SchemeBase<EvaluationEngine> currentPeakScheme("Current peak", "", exportables, DataExporter::SchemeTypes::SINGLE_ITEM);
  DataExporter::SchemeBase<EvaluationEngine> peakList("List of peaks", "", listExportables, DataExporter::SchemeTypes::LIST,
                       [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap) {
    for (const PeakContext &peakCtx : exportee->m_allPeaks) {
      for (const SelectedExportableBase *se : seMap) {
        qDebug() << peakCtx.peakName << se->name() << se->value(&peakCtx);
      }
    }

    return true;
  });


}
