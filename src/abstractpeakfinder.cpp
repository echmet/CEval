#include "abstractpeakfinder.h"

PeakFinderResults::PeakFinderResults() :
  tPiCoarse(-1),
  m_valid(false)
{
}

PeakFinderResults::PeakFinderResults(const PeakFinderResults &other) :
  fromIndex(other.fromIndex),
  toIndex(other.toIndex),
  indexAtMax(other.indexAtMax),
  fromPeakIndex(other.fromPeakIndex),
  toPeakIndex(other.toPeakIndex),
  tPiCoarse(other.tPiCoarse),
  maxY(other.maxY),
  minY(other.minY),
  peakFromX(other.peakFromX),
  peakFromY(other.peakFromY),
  peakToX(other.peakToX),
  peakToY(other.peakToY),
  peakX(other.peakX),
  peakHeight(other.peakHeight),
  peakHeightBaseline(other.peakHeightBaseline),
  noiseRefPoint(other.noiseRefPoint),
  slopeRefPoint(other.slopeRefPoint),
  twPLeft(other.twPLeft),
  twPRight(other.twPRight),
  baselineSlope(other.baselineSlope),
  baselineIntercept(other.baselineIntercept),
  noise(other.noise),
  slopeThreshold(other.slopeThreshold),
  slopeWindow(other.slopeWindow),
  m_valid(other.m_valid)
{
  if (other.seriesA == nullptr)
    seriesA = nullptr;
  else
    seriesA = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>(*other.seriesA));

  if (other.seriesB == nullptr)
    seriesB = nullptr;
  else
    seriesB = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>(*other.seriesB));
}

bool PeakFinderResults::isValid() const
{
  return m_valid;
}

void PeakFinderResults::validate()
{
  m_valid = true;
}
