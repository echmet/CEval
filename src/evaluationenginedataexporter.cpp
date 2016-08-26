#include "dataexporter/exporterelems.h"
#include "doubletostringconvertor.h"
#include "evaluationengine.h"

class PeakDimsTuple {
public:
  explicit PeakDimsTuple() : left(0.0), right(0.0), full(0.0) {};
  PeakDimsTuple(const double left, const double right, const double full) :
    left(left), right(right), full(full) {}

  const double left;
  const double right;
  const double full;

};
Q_DECLARE_METATYPE(PeakDimsTuple)

#define MAKE_EXPORTABLE(map, TT, name, getter) \
  if (map.contains(name)) throw DataExporter::ExportableExistsException(); \
  map.insert(name, new DataExporter::Exportable<TT>(name, \
    [](const TT *me) { \
      return QVariant(DoubleToStringConvertor::convert(me->getter)); \
  }))

#define MAKE_EXPORTABLE_DIMS(map, TT, name, dimitem) \
  if (map.contains(name)) throw DataExporter::ExportableExistsException(); \
  map.insert(name, new DataExporter::Exportable<TT>(name, \
    [](const TT *me) { \
      return QVariant::fromValue<PeakDimsTuple>(PeakDimsTuple(me->m_resultsNumericValues.at(EvaluationResultsItems::Floating::dimitem##_LEFT), \
                                                              me->m_resultsNumericValues.at(EvaluationResultsItems::Floating::dimitem##_RIGHT), \
                                                              me->m_resultsNumericValues.at(EvaluationResultsItems::Floating::dimitem##_FULL))); \
  }))

#define MAKE_EXPORTABLE_STR(map, TT, name, getter) \
  if (map.contains(name)) throw DataExporter::ExportableExistsException(); \
  map.insert(name, new DataExporter::Exportable<TT>(name, \
    [](const TT *me) { \
      return QVariant(me->getter); \
  }))

#define MAKE_SELECTED_EXPORTABLE_CTC(bmap, map, name, position) \
  if (!bmap.contains(name)) throw DataExporter::UnknownExportableException(); \
  map.insert(name, new DataExporter::SelectedExportable(bmap.value(name), position, name));

void EvaluationEngine::initClipboardExporter()
{
  DataExporter::ExportablesMap eof;
  DataExporter::ExportablesMap hvl;
  DataExporter::ExportablesMap peak;
  DataExporter::ExportablesMap peakDims;

  MAKE_EXPORTABLE(eof, EvaluationEngine, "v (1e-3 m/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY));
  MAKE_EXPORTABLE(eof, EvaluationEngine, "v! (1e-3 m/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY_EFF));
  MAKE_EXPORTABLE(eof, EvaluationEngine, "u (1e-9 m.m/V/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_MOBILITY));

  MAKE_EXPORTABLE(hvl, EvaluationEngine, "a0", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0));
  MAKE_EXPORTABLE(hvl, EvaluationEngine, "a1", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1));
  MAKE_EXPORTABLE(hvl, EvaluationEngine, "a2", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2));
  MAKE_EXPORTABLE(hvl, EvaluationEngine, "a3", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3));
  MAKE_EXPORTABLE(hvl, EvaluationEngine, "S", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_S));
  MAKE_EXPORTABLE(hvl, EvaluationEngine, "a1 u! (1e-9 m.m/V/s)", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1));

  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak from X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak from Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak to X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak to Y", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak max at X", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Peak height", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "v (1e-3 m/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "v! (1e-3 m/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "u! (1e-9 m.m/V/s)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "Area (Units.min)", m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  MAKE_EXPORTABLE(peak, EvaluationEngine, "t USP", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_TUSP));

  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "Width 1/2 (min)", WIDTH_HALF_MIN);
  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "Sigma (min)", SIGMA_MIN);
  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "Width 1/2 (m)", WIDTH_HALF_MET);
  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "Sigma (m)", SIGMA_MET);
  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "N", N);
  MAKE_EXPORTABLE_DIMS(peakDims, EvaluationEngine, "HETP", N_H);

  auto peakDimsExecutor = [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap, DataExporter::AbstractExporterBackend &backend, const uint32_t opts) -> bool {
    typedef DataExporter::AbstractExporterBackend::Cell Cell;
    (void)opts;
    if (!exportee->isContextValid())
      return false;

    int blockCtr = 0;
    for (const DataExporter::SelectedExportable *se : seMap) {
      backend.addCell(new Cell(se->displayName + " left", "", Cell::SINGLE), blockCtr, 0);
      backend.addCell(new Cell(se->displayName + " right", "", Cell::SINGLE), blockCtr, 1);
      backend.addCell(new Cell(se->displayName + " full", "", Cell::SINGLE), blockCtr, 2);

      blockCtr++;

      PeakDimsTuple dims = se->value(exportee).value<PeakDimsTuple>();
      backend.addCell(new Cell(DoubleToStringConvertor::convert(dims.left), "", Cell::SINGLE), blockCtr, 0);
      backend.addCell(new Cell(DoubleToStringConvertor::convert(dims.right), "", Cell::SINGLE), blockCtr, 1);
      backend.addCell(new Cell(DoubleToStringConvertor::convert(dims.full), "", Cell::SINGLE), blockCtr, 2);

      blockCtr++;
    }

    return backend.exportData();
  };

  m_ctcEofSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("EOF", "", eof);
  m_ctcHvlSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("HVL", "", hvl);
  m_ctcPeakSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("Peak", "", peak);
  m_ctcPeakDimsSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("PeakDims", "", peakDims, peakDimsExecutor);

  DataExporter::SelectedExportablesMap seEof;
  DataExporter::SelectedExportablesMap seHvl;
  DataExporter::SelectedExportablesMap sePeak;
  DataExporter::SelectedExportablesMap sePeakDims;

  MAKE_SELECTED_EXPORTABLE_CTC(eof, seEof, "v (1e-3 m/s)", 0);
  MAKE_SELECTED_EXPORTABLE_CTC(eof, seEof, "v! (1e-3 m/s)", 1);
  MAKE_SELECTED_EXPORTABLE_CTC(eof, seEof, "u (1e-9 m.m/V/s)", 2);

  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "a0", 0);
  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "a1", 1);
  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "a2", 2);
  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "a3", 3);
  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "S", 4);
  MAKE_SELECTED_EXPORTABLE_CTC(hvl, seHvl, "a1 u! (1e-9 m.m/V/s)", 5);

  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak from X", 0);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak from Y", 1);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak to X", 2);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak to Y", 3);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak max at X", 4);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Peak height", 5);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "v (1e-3 m/s)", 6);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "v! (1e-3 m/s)", 7);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "u! (1e-9 m.m/V/s)", 8);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "Area (Units.min)", 9);
  MAKE_SELECTED_EXPORTABLE_CTC(peak, sePeak, "t USP", 10);

  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "Width 1/2 (min)", 0);
  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "Sigma (min)", 1);
  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "Width 1/2 (m)", 2);
  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "Sigma (m)", 3);
  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "N", 4);
  MAKE_SELECTED_EXPORTABLE_CTC(peakDims, sePeakDims, "HETP", 5);

  m_ctcEofScheme = new DataExporter::Scheme("EOF", seEof, m_ctcEofSchemeBase, DataExporter::Globals::DataArrangement::VERTICAL);
  m_ctcHvlScheme = new DataExporter::Scheme("HVL", seHvl, m_ctcHvlSchemeBase, DataExporter::Globals::DataArrangement::VERTICAL);
  m_ctcPeakScheme = new DataExporter::Scheme("Peak", sePeak, m_ctcPeakSchemeBase, DataExporter::Globals::DataArrangement::VERTICAL);
  m_ctcPeakDimsScheme = new DataExporter::Scheme("PeakDims", sePeakDims, m_ctcPeakDimsSchemeBase, DataExporter::Globals::DataArrangement::VERTICAL);
}

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
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL a0", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL a1", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL a2", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL a3", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL S", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_S));
  MAKE_EXPORTABLE(currentPeakExportables, EvaluationEngine, "HVL effective mobility", m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1));
  MAKE_EXPORTABLE_STR(currentPeakExportables, EvaluationEngine, "Datafile", m_currentDataContextKey);


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
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL a0", hvlValues.at(HVLFitResultsItems::Floating::HVL_A0));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL a1", hvlValues.at(HVLFitResultsItems::Floating::HVL_A1));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL a2", hvlValues.at(HVLFitResultsItems::Floating::HVL_A2));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL a3", hvlValues.at(HVLFitResultsItems::Floating::HVL_A3));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL S", hvlValues.at(HVLFitResultsItems::Floating::HVL_S));
  MAKE_EXPORTABLE(peakListExportables, PeakContext, "HVL effective mobility", hvlValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1));
  MAKE_EXPORTABLE_STR(peakListExportables, EvaluationEngine, "Datafile", m_currentDataContextKey);

  auto peakListExecutor = [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap, DataExporter::AbstractExporterBackend &backend, const uint32_t opts) -> bool {
    typedef DataExporter::AbstractExporterBackend::Cell Cell;
    (void)opts;
    if (!exportee->isContextValid())
      return false;

    if (exportee->m_allPeaks.size() < 2)
      return true;

    int blockCtr = 0;
    for (int idx = 1; idx < exportee->m_allPeaks.size(); idx++) {
      const PeakContext *pCtx = &exportee->m_allPeaks.at(idx);
      backend.addCell(new Cell(pCtx->peakName, "", DataExporter::AbstractExporterBackend::Cell::SINGLE), blockCtr++, 0);

      for (const DataExporter::SelectedExportable *se : seMap) {
        try {
          backend.addCell(new Cell(se->name(), se->value(pCtx)), blockCtr, se->position);
        } catch (DataExporter::InvalidExportableException &) {
          /* Other possibilities are either "EOF time" or "Datafile", both take EvaluationEngine as
           * the exportee */
          backend.addCell(new Cell(se->displayName, se->value(exportee)), blockCtr, se->position);
        }
      }
      blockCtr++;
    }

    return backend.exportData();
  };

  auto peakListTableExecutor = [](const EvaluationEngine *exportee, const DataExporter::SelectedExportablesMap &seMap, DataExporter::AbstractExporterBackend &backend, const uint32_t opts) -> bool {
    typedef DataExporter::AbstractExporterBackend::Cell Cell;
    (void)opts;
    if (!exportee->isContextValid())
      return false;

    if (exportee->m_allPeaks.size() < 2)
      return true;

    const bool exclude_header = opts & DataExporter::SchemeBaseRoot::EXCLUDE_HEADER;
    const int shift = (exclude_header && (backend.arrangement() == DataExporter::Globals::DataArrangement::VERTICAL)) ? 1 : 0;
    const int topBlockCtr = (exclude_header && (backend.arrangement() == DataExporter::Globals::DataArrangement::HORIZONTAL)) ? 0 : 1;

    /* TODO: Remove the utilitary appending hack and header detection once we have a mass-processing mode.
     * Executor function should NOT! be aware of data arrangement as the backend takes care of the appropriate
     * transformation */

    /* Output peak names along one direction */
    if (!(exclude_header && (backend.arrangement() == DataExporter::Globals::DataArrangement::HORIZONTAL))) {
      for (int idx = 1; idx < exportee->m_allPeaks.size(); idx++) {
        const PeakContext *pCtx = &exportee->m_allPeaks.at(idx);
        backend.addCell(new Cell(pCtx->peakName, "", DataExporter::AbstractExporterBackend::Cell::SINGLE | DataExporter::AbstractExporterBackend::Cell::CAPTION),
                        0, idx - shift);
      }
    }

    if (!(exclude_header && (backend.arrangement() == DataExporter::Globals::DataArrangement::VERTICAL))) {
      /* Output value names along the other direction */
      for (const DataExporter::SelectedExportable *se : seMap)
        backend.addCell(new Cell(se->name(), "", DataExporter::AbstractExporterBackend::Cell::SINGLE | DataExporter::AbstractExporterBackend::Cell::CAPTION),
                        se->position + topBlockCtr, 0);
    }

    /* Output the actual values */
    for (int idx = 1; idx < exportee->m_allPeaks.size(); idx++) {
      const PeakContext *pCtx = &exportee->m_allPeaks.at(idx);

      for (const DataExporter::SelectedExportable *se : seMap) {
        QString s;
        try {
          s = se->value(pCtx).toString();
        } catch (DataExporter::InvalidExportableException &) {
          /* Other possibilities are either "EOF time" or "Datafile", both take EvaluationEngine as
           * the exportee */
            s = se->value(exportee).toString();
        }

        backend.addCell(new Cell(s, "", DataExporter::AbstractExporterBackend::Cell::SINGLE), se->position + topBlockCtr, idx - shift);
      }
    }

    return backend.exportData();
  };

  DataExporter::SchemeBase<EvaluationEngine> *currentPeakSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("Current peak", tr("Export current peak"), currentPeakExportables);
  DataExporter::SchemeBase<EvaluationEngine> *peakListSchemeBase = new DataExporter::SchemeBase<EvaluationEngine>("List of peaks", tr("Export complete list of evaluated peaks"), peakListExportables, peakListExecutor);
  DataExporter::SchemeBase<EvaluationEngine> *peakListTableShemeBase = new DataExporter::SchemeBase<EvaluationEngine>("List of peaks (simple table)", tr("Export complete list of peaks arranged in a table.\n\nThis scheme is reasonably compatible with data appending hack."), peakListExportables, peakListTableExecutor, true);

  if (!m_dataExporter.registerSchemeBase(currentPeakSchemeBase)) {
    delete currentPeakSchemeBase;
    delete peakListSchemeBase;
    delete peakListTableShemeBase;
    return false;
  }
  if (!m_dataExporter.registerSchemeBase(peakListSchemeBase)) {
    delete peakListSchemeBase;
    delete peakListTableShemeBase;
    return false;
  }
  if (!m_dataExporter.registerSchemeBase(peakListTableShemeBase)) {
    delete peakListTableShemeBase;
    return false;
  }

  return true;
}
