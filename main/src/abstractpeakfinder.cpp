#include "abstractpeakfinder.h"

PeakFinderResults::Result::Result() :
  fromIndex(0),
  toIndex(0),
  peakFromX(0),
  peakFromY(0),
  peakToX(0),
  peakToY(0),
  m_valid(false)
{
}

PeakFinderResults::Result::Result(const int fromIndex, const int toIndex,
                                  const double peakFromX, const double peakFromY,
                                  const double peakToX, const double peakToY) :
  fromIndex(fromIndex),
  toIndex(toIndex),
  peakFromX(peakFromX),
  peakFromY(peakFromY),
  peakToX(peakToX),
  peakToY(peakToY),
  m_valid(true)
{
}

PeakFinderResults::Result::Result(const Result &other) :
  fromIndex(other.fromIndex),
  toIndex(other.toIndex),
  peakFromX(other.peakFromX),
  peakFromY(other.peakFromY),
  peakToX(other.peakToX),
  peakToY(other.peakToY),
  m_valid(other.m_valid)
{
}

PeakFinderResults::Result::~Result()
{
}

bool PeakFinderResults::Result::isValid() const
{
  return m_valid;
}

PeakFinderResults::PeakFinderResults() :
  results(QVector<std::shared_ptr<Result>>())
{
}

PeakFinderResults::PeakFinderResults(const QVector<std::shared_ptr<Result>> &results) :
  results(results)
{
}

PeakFinderResults::PeakFinderResults(const PeakFinderResults &other) :
  results(other.results)
{
}

PeakFinderResults::~PeakFinderResults()
{
}

PeakFinderResults * PeakFinderResults::copy() const
{
  return new PeakFinderResults(*this);
}

PeakFinderResults & PeakFinderResults::operator=(const PeakFinderResults &other)
{
  const_cast<QVector<std::shared_ptr<Result>>&>(results) = other.results;

  return *this;
}
