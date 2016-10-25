#include "evaluationengine.h"

EvaluationEngine::PeakContext::PeakContext() :
  afSettings(AssistedFinderSettings()),
  finderResults(std::make_shared<PeakFinderResults::Result>()),
  baselineSlope(0.0),
  baselineIntercept(0.0)
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                           const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                           const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitFixedValues,
                                           const AssistedFinderSettings &afSettings,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults, const double baselineSlope, const double baselineIntercept,
                                           const QVector<QPointF> &hvlPlot) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitFixedValues(hvlFitFixedValues),
  afSettings(afSettings),
  finderResults(finderResults),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot)
{
}

EvaluationEngine::PeakContext::PeakContext(const PeakContext &other) :
  resultsValues(other.resultsValues),
  hvlValues(other.hvlValues),
  hvlFitIntValues(other.hvlFitIntValues),
  afSettings(other.afSettings),
  finderResults(other.finderResults),
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
  const_cast<double&>(baselineSlope) = other.baselineSlope;
  const_cast<double&>(baselineIntercept) = other.baselineIntercept;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;
  const_cast<std::shared_ptr<PeakFinderResults::Result>&>(finderResults) = other.finderResults;
  const_cast<AssistedFinderSettings&>(afSettings) = other.afSettings;

  return *this;
}
