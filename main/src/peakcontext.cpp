#include "evaluationengine.h"

EvaluationEngine::PeakContext::PeakContext() :
  afContext(AssistedFinderContext()),
  finderResults(std::make_shared<PeakFinderResults::Result>()),
  baselineSlope(0.0),
  baselineIntercept(0.0)
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                           const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                           const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                                           const AssistedFinderContext &afContext,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults, const double baselineSlope, const double baselineIntercept,
                                           const QVector<QPointF> &hvlPlot,
                                           const QVector<QPointF> &hvlPlotExtrapolated) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitBooleanValues(hvlFitBooleanValues),
  afContext(afContext),
  finderResults(finderResults),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot),
  hvlPlotExtrapolated(hvlPlotExtrapolated)
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                           const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                           const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                                           const AssistedFinderContext &afContext,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults, const double baselineSlope, const double baselineIntercept,
                                           QVector<QPointF> &&hvlPlot,
                                           QVector<QPointF> &&hvlPlotExtrapolated) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitBooleanValues(hvlFitBooleanValues),
  afContext(afContext),
  finderResults(finderResults),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot),
  hvlPlotExtrapolated(hvlPlotExtrapolated)
{
}

EvaluationEngine::PeakContext::PeakContext(const PeakContext &other) :
  resultsValues(other.resultsValues),
  hvlValues(other.hvlValues),
  hvlFitIntValues(other.hvlFitIntValues),
  hvlFitBooleanValues(other.hvlFitBooleanValues),
  afContext(other.afContext),
  finderResults(other.finderResults),
  baselineSlope(other.baselineSlope),
  baselineIntercept(other.baselineIntercept),
  hvlPlot(other.hvlPlot),
  hvlPlotExtrapolated(other.hvlPlotExtrapolated)
{
}

void EvaluationEngine::PeakContext::updateHvlData(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &inHvlValues,
                                                  const MappedVectorWrapper<int, HVLFitParametersItems::Int> &inHvlFitIntValues,
                                                  const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &inHvlFitBooleanValues)
{
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = inHvlValues;
  const_cast<MappedVectorWrapper<int, HVLFitParametersItems::Int>&>(hvlFitIntValues) = inHvlFitIntValues;
  const_cast<MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>&>(hvlFitBooleanValues) = inHvlFitBooleanValues;
}

void EvaluationEngine::PeakContext::updateHvlPlot(const QVector<QPointF> &plot, const QVector<QPointF> &plotExtrapolated)
{
  const_cast<QVector<QPointF>&>(hvlPlot) = plot;
  const_cast<QVector<QPointF>&>(hvlPlotExtrapolated) = plotExtrapolated;
}

void EvaluationEngine::PeakContext::updateHvlPlot(QVector<QPointF> &&plot, QVector<QPointF> &&plotExtrapolated)
{
  const_cast<QVector<QPointF>&>(hvlPlot) = plot;
  const_cast<QVector<QPointF>&>(hvlPlotExtrapolated) = plotExtrapolated;
}

EvaluationEngine::PeakContext &EvaluationEngine::PeakContext::operator=(const PeakContext &other)
{
  const_cast<MappedVectorWrapper<double, EvaluationResultsItems::Floating>&>(resultsValues) = other.resultsValues;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = other.hvlValues;
  const_cast<MappedVectorWrapper<int, HVLFitParametersItems::Int>&>(hvlFitIntValues) = other.hvlFitIntValues;
  const_cast<MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>&>(hvlFitBooleanValues) = other.hvlFitBooleanValues;
  const_cast<double&>(baselineSlope) = other.baselineSlope;
  const_cast<double&>(baselineIntercept) = other.baselineIntercept;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;
  const_cast<QVector<QPointF>&>(hvlPlotExtrapolated) = other.hvlPlotExtrapolated;
  const_cast<std::shared_ptr<PeakFinderResults::Result>&>(finderResults) = other.finderResults;
  const_cast<AssistedFinderContext&>(afContext) = other.afContext;

  return *this;
}

EvaluationEngine::PeakContext &EvaluationEngine::PeakContext::operator=(PeakContext &&other)
{
  const_cast<MappedVectorWrapper<double, EvaluationResultsItems::Floating>&>(resultsValues) = other.resultsValues;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = other.hvlValues;
  const_cast<MappedVectorWrapper<int, HVLFitParametersItems::Int>&>(hvlFitIntValues) = other.hvlFitIntValues;
  const_cast<MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>&>(hvlFitBooleanValues) = other.hvlFitBooleanValues;
  const_cast<double&>(baselineSlope) = other.baselineSlope;
  const_cast<double&>(baselineIntercept) = other.baselineIntercept;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;
  const_cast<QVector<QPointF>&>(hvlPlotExtrapolated) = other.hvlPlotExtrapolated;
  const_cast<std::shared_ptr<PeakFinderResults::Result>&>(finderResults) = other.finderResults;
  const_cast<AssistedFinderContext&>(afContext) = other.afContext;

  return *this;
}
