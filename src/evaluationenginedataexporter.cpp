#include "dataexporter.h"
#include "evaluationengine.h"

/*
#define MAKE_EXPORTABLE(map, TT, name, getter) \
  map.insert(name, new Exportable<TT>(name, \
    [](const TT *me) { \
      if (!me->isContextValid()) \
        return QVariant(); \
      return QVariant(me->getter); \
  }))
*/

#define MAKE_EXPORTABLE(map, TT, name, getter) \
  map.insert(name, new DataExporter::Exportable<TT>(name, \
    [](const TT *me) { \
      return QVariant(me->getter); \
  }))

void EvaluationEngine::initDataExporter()
{
  /* THE OBVIOUS PART */
  {
  DataExporter::ExportablesMap exportables;

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

  DataExporter::SchemeBase<EvaluationEngine> *cps = new DataExporter::SchemeBase<EvaluationEngine>("Current peak", "", exportables, DataExporter::SchemeTypes::SINGLE_ITEM);
  DataExporter::SelectedExportablesMap selExps;
  selExps.insert("Peak area", new DataExporter::SelectedExportable(exportables.value("Peak area")));
  selExps.insert("Peak height", new DataExporter::SelectedExportable(exportables.value("Peak height")));

  m_dataExporter.schemes.push_back(new DataExporter::Scheme("Current peak", selExps, cps));
  }

  /* THE SCARY PART */
  {
  DataExporter::ExportablesMap listExportables;
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "EOF velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak effective velocity", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak effective mobility", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak area", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  MAKE_EXPORTABLE(listExportables, EvaluationEngine, "EOF time", m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF));

  auto executor = [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap) -> bool {
    if (exportee->m_allPeaks.size() < 1) {
      qDebug() << "No peaks in list";
      return true;
    }

    for (const PeakContext &ctx : exportee->m_allPeaks) {
      const PeakContext *pCtx = &ctx;
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
    qDebug() << eof->name() << eof->value(exportee);

    return true;
  };

  DataExporter::SchemeBase<EvaluationEngine> *peaksBase = new DataExporter::SchemeBase<EvaluationEngine>("List of peaks", "", listExportables, DataExporter::SchemeTypes::LIST, executor);

  DataExporter::SelectedExportablesMap selExps;
  selExps.insert("Peak area", new DataExporter::SelectedExportable(listExportables.value("Peak area")));
  selExps.insert("Peak height", new DataExporter::SelectedExportable(listExportables.value("Peak height")));
  selExps.insert("EOF time", new DataExporter::SelectedExportable(listExportables.value("EOF time")));

  m_dataExporter.schemes.push_back(new DataExporter::Scheme("List of peaks", selExps, peaksBase));
  }
}
