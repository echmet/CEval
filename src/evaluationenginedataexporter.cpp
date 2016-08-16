#include "dataexporter.h"
#include "evaluationengine.h"

#define MAKE_EXPORTABLE(map, TT, name, getter) \
  if (map.contains(name)) throw DataExporter::ExportableExistsException(); \
  map.insert(name, new DataExporter::Exportable<TT>(name, \
    [](const TT *me) { \
      return QVariant(me->getter); \
  }))

bool EvaluationEngine::initDataExporter()
{
  DataExporter::ExportablesMap currentPeakExportables;
  DataExporter::ExportablesMap peakListExportables;

  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "EOF velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak effective velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak effective mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "EOF time", m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF));

  MAKE_EXPORTABLE(peakListExportables, PeakContext, "EOF velocity", resultsValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak from X", resultsValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak from Y", resultsValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak to X", resultsValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak to Y", resultsValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak max at X", resultsValues.at(EvaluationResultsItems::Floating::PEAK_X));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak velocity", resultsValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak effective velocity", resultsValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak mobility", resultsValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak effective mobility", resultsValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak area", resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "Peak height", resultsValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  MAKE_EXPORTABLE(peakListExportables, EvaluationEngine, "EOF time", m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF));

  auto peakListExecutor = [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap) -> bool {
    if (exportee->m_allPeaks.size() < 1) {
      qDebug() << "No peaks in list";
      return true;
    }

    for (int idx = 1; idx < exportee->m_allPeaks.size(); idx++) {
      const PeakContext *pCtx = &exportee->m_allPeaks.at(idx);
      qDebug() << pCtx->peakName;

      for (const DataExporter::SelectedExportable *se : seMap) {
        try {
          qDebug() << se->name() << se->value(pCtx);
        } catch (DataExporter::InvalidExportableException &) {
          /* We expect this to happen */
        }
      }
    }

    const DataExporter::SelectedExportable *eof = seMap.value("EOF time");
    if (eof != nullptr)
      qDebug() << eof->name() << eof->value(exportee);

    return true;
  };

  DataExporter::SchemeBase<EvaluationEngine> *currentPeakSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("Current peak", "Export current peak", currentPeakExportables, DataExporter::SchemeTypes::SINGLE_ITEM);
  DataExporter::SchemeBase<EvaluationEngine> *peakListSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("List of peaks", "Export complete list of evaluated peaks", peakListExportables, DataExporter::SchemeTypes::LIST, peakListExecutor);

  if (!m_dataExporter.registerSchemeBase(currentPeakSchemeBase))
    return false;
  if (!m_dataExporter.registerSchemeBase(peakListSchemeBase))
    return false;

  return true;
}
