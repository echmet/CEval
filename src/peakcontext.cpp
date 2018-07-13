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
                                           const MappedVectorWrapper<double, SNRItems::Floating> &snrValues,
                                           const AssistedFinderContext &afContext,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults, const double baselineSlope, const double baselineIntercept,
                                           const QVector<QPointF> &hvlPlot,
                                           const QVector<QPointF> &hvlPlotExtrapolated,
                                           const QPointF &noiseFrom,
                                           const QPointF &noiseTo) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitBooleanValues(hvlFitBooleanValues),
  afContext(afContext),
  finderResults(finderResults),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot),
  hvlPlotExtrapolated(hvlPlotExtrapolated),
  snrValues(snrValues),
  noiseFrom(noiseFrom),
  noiseTo(noiseTo)
{
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                           const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                           const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                                           const MappedVectorWrapper<double, SNRItems::Floating> &snrValues,
                                           const AssistedFinderContext &afContext,
                                           const std::shared_ptr<PeakFinderResults::Result> &finderResults, const double baselineSlope, const double baselineIntercept,
                                           QVector<QPointF> &&hvlPlot,
                                           QVector<QPointF> &&hvlPlotExtrapolated,
                                           const QPointF &noiseFrom,
                                           const QPointF &noiseTo) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitBooleanValues(hvlFitBooleanValues),
  afContext(afContext),
  finderResults(finderResults),
  baselineSlope(baselineSlope),
  baselineIntercept(baselineIntercept),
  hvlPlot(hvlPlot),
  hvlPlotExtrapolated(hvlPlotExtrapolated),
  snrValues(snrValues),
  noiseFrom(noiseFrom),
  noiseTo(noiseTo)
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
  hvlPlotExtrapolated(other.hvlPlotExtrapolated),
  snrValues(other.snrValues),
  noiseFrom(other.noiseFrom),
  noiseTo(other.noiseTo)
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

void EvaluationEngine::PeakContext::clearHvlExtrapolation()
{
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues)[HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_VARIANCE] = 0.0;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues)[HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_SIGMA] = 0.0;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues)[HVLFitResultsItems::Floating::HVL_EXTRAPOLATED_MEAN] = 0.0;
  const_cast<QVector<QPointF>&>(hvlPlotExtrapolated).clear();
}

bool EvaluationEngine::PeakContext::hasNoise() const
{
  return !(noiseFrom.isNull() && noiseTo.isNull());
}

void EvaluationEngine::PeakContext::setNoise(const QPointF &_noiseFrom, const QPointF &_noiseTo, MappedVectorWrapper<double, SNRItems::Floating> &_snrValues)
{
  const_cast<QPointF&>(this->noiseFrom) = _noiseFrom;
  const_cast<QPointF&>(this->noiseTo) = _noiseTo;
  const_cast<MappedVectorWrapper<double, SNRItems::Floating>&>(this->snrValues) = _snrValues;
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
  const_cast<MappedVectorWrapper<double, SNRItems::Floating>&>(snrValues) = other.snrValues;
  const_cast<QPointF&>(noiseFrom) = other.noiseFrom;
  const_cast<QPointF&>(noiseTo) = other.noiseTo;

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
  const_cast<MappedVectorWrapper<double, SNRItems::Floating>&>(snrValues) = other.snrValues;
  const_cast<QPointF&>(noiseFrom) = other.noiseFrom;
  const_cast<QPointF&>(noiseTo) = other.noiseTo;

  return *this;
}
