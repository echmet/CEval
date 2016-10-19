#include "evaluationengine.h"

EvaluationEngine::PeakContext::PeakContext() :
  windowUnit(EvaluationParametersItems::ComboWindowUnits::LAST_INDEX),
  showWindow(EvaluationParametersItems::ComboShowWindow::LAST_INDEX),
  baselineAlgorithm(EvaluationParametersItems::ComboBaselineAlgorithm::LAST_INDEX),
  finderResults(std::make_shared<PeakFinderResults::Result>()),
  peakIndex(-1),
  baselineSlope(0.0),
  baselineIntercept(0.0)
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                           const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                           const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitFixedValues,
                                           const EvaluationParametersItems::ComboWindowUnits windowUnit, const EvaluationParametersItems::ComboShowWindow showWindow,
                                           const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults,
                                           const int peakIndex, const double baselineSlope, const double baselineIntercept,
                                           const QVector<QPointF> &hvlPlot) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitFixedValues(hvlFitFixedValues),
  windowUnit(windowUnit), showWindow(showWindow), baselineAlgorithm(baselineAlgorithm),
  finderResults(finderResults),
  peakIndex(peakIndex),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot)
{
}

EvaluationEngine::PeakContext::PeakContext(const PeakContext &other) :
  resultsValues(other.resultsValues),
  hvlValues(other.hvlValues),
  hvlFitIntValues(other.hvlFitIntValues),
  hvlFitFixedValues(other.hvlFitFixedValues),
  windowUnit(other.windowUnit),
  showWindow(other.showWindow),
  baselineAlgorithm(other.baselineAlgorithm),
  finderResults(other.finderResults),
  peakIndex(other.peakIndex),
  baselineSlope(other.baselineSlope),
  baselineIntercept(other.baselineIntercept),
  hvlPlot(other.hvlPlot)
{
}

void EvaluationEngine::PeakContext::updateHvlData(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &inHvlValues,
                                                  const MappedVectorWrapper<int, HVLFitParametersItems::Int> &inHvlFitIntValues,
                                                  const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &inHvlFitFixedValues)
{
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = inHvlValues;
  const_cast<MappedVectorWrapper<int, HVLFitParametersItems::Int>&>(hvlFitIntValues) = inHvlFitIntValues;
  const_cast<MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>&>(hvlFitFixedValues) = inHvlFitFixedValues;
}

void EvaluationEngine::PeakContext::updateHvlPlot(const QVector<QPointF> &plot)
{
  const_cast<QVector<QPointF>&>(hvlPlot) = plot;
}

EvaluationEngine::PeakContext &EvaluationEngine::PeakContext::operator=(const PeakContext &other)
{
  const_cast<MappedVectorWrapper<double, EvaluationResultsItems::Floating>&>(resultsValues) = other.resultsValues;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = other.hvlValues;
  const_cast<MappedVectorWrapper<int, HVLFitParametersItems::Int>&>(hvlFitIntValues) = other.hvlFitIntValues;
  const_cast<MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>&>(hvlFitFixedValues) = other.hvlFitFixedValues;
  const_cast<EvaluationParametersItems::ComboWindowUnits&>(windowUnit) = other.windowUnit;
  const_cast<EvaluationParametersItems::ComboShowWindow&>(showWindow) = other.showWindow;
  const_cast<EvaluationParametersItems::ComboBaselineAlgorithm&>(baselineAlgorithm) = other.baselineAlgorithm;
  const_cast<int&>(peakIndex) = other.peakIndex;
  const_cast<double&>(baselineSlope) = other.baselineSlope;
  const_cast<double&>(baselineIntercept) = other.baselineIntercept;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;
  const_cast<std::shared_ptr<PeakFinderResults::Result>&>(finderResults) = other.finderResults;

  return *this;
}
