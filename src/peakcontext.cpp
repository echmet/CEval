#include "evaluationengine.h"

EvaluationEngine::PeakContext::PeakContext() :
  windowUnit(EvaluationParametersItems::ComboWindowUnits::LAST_INDEX),
  showWindow(EvaluationParametersItems::ComboShowWindow::LAST_INDEX),
  baselineAlgorithm(EvaluationParametersItems::ComboBaselineAlgorithm::LAST_INDEX),
  finderResults(new PeakFinderResults()),
  peakIndex(-1),
  baselineSlope(0.0),
  baselineIntercept(0.0),
  peakName("")
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &autoValues,
                                           const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &boolValues,
                                           const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &floatingValues,
                                           const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues, const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const EvaluationParametersItems::ComboWindowUnits windowUnit, const EvaluationParametersItems::ComboShowWindow showWindow,
                                           const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                                           const PeakFinderResults *finderResults,
                                           const int peakIndex, const double baselineSlope, const double baselineIntercept,
                                           const QVector<QPointF> &hvlPlot) :
  autoValues(autoValues), boolValues(boolValues), floatingValues(floatingValues),
  resultsValues(resultsValues), hvlValues(hvlValues),
  windowUnit(windowUnit), showWindow(showWindow), baselineAlgorithm(baselineAlgorithm),
  finderResults(finderResults),
  peakIndex(peakIndex),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot), peakName("")
{
}

EvaluationEngine::PeakContext::PeakContext(const PeakContext &other) :
  autoValues(other.autoValues),
  boolValues(other.boolValues),
  floatingValues(other.floatingValues),
  resultsValues(other.resultsValues),
  hvlValues(other.hvlValues),
  windowUnit(other.windowUnit),
  showWindow(other.showWindow),
  baselineAlgorithm(other.baselineAlgorithm),
  finderResults(other.finderResults->copy()),
  peakIndex(other.peakIndex),
  baselineSlope(other.baselineSlope),
  baselineIntercept(other.baselineIntercept),
  hvlPlot(other.hvlPlot),
  peakName(other.peakName)
{
}

EvaluationEngine::PeakContext::~PeakContext()
{
  delete finderResults;
}

void EvaluationEngine::PeakContext::setPeakName(const QString &name)
{
  const_cast<QString&>(peakName) = name;
}


EvaluationEngine::PeakContext &EvaluationEngine::PeakContext::operator=(const PeakContext &other)
{
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Auto>&>(autoValues) = other.autoValues;
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Boolean>&>(boolValues) = other.boolValues;
  const_cast<MappedVectorWrapper<double, EvaluationParametersItems::Floating>&>(floatingValues) = other.floatingValues;
  const_cast<MappedVectorWrapper<double, EvaluationResultsItems::Floating>&>(resultsValues) = other.resultsValues;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = other.hvlValues;
  const_cast<EvaluationParametersItems::ComboWindowUnits&>(windowUnit) = other.windowUnit;
  const_cast<EvaluationParametersItems::ComboShowWindow&>(showWindow) = other.showWindow;
  const_cast<EvaluationParametersItems::ComboBaselineAlgorithm&>(baselineAlgorithm) = other.baselineAlgorithm;
  const_cast<int&>(peakIndex) = other.peakIndex;
  const_cast<double&>(baselineSlope) = other.baselineSlope;
  const_cast<double&>(baselineIntercept) = other.baselineIntercept;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;
  const_cast<QString&>(peakName) = other.peakName;

  delete finderResults;
  finderResults = other.finderResults->copy();

  return *this;
}
