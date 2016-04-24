#include "abstractpeakfinder.h"

PeakFinderResults::PeakFinderResults() :
  m_valid(false)
{
}

PeakFinderResults::PeakFinderResults(const PeakFinderResults &other) :
  fromIndex(other.fromIndex),
  toIndex(other.toIndex),
  peakFromX(other.peakFromX),
  peakFromY(other.peakFromY),
  peakToX(other.peakToX),
  peakToY(other.peakToY),
  m_valid(other.m_valid)
{
}

PeakFinderResults::~PeakFinderResults()
{
}

PeakFinderResults *PeakFinderResults::copy() const
{
  return new PeakFinderResults(*this);
}

bool PeakFinderResults::isValid() const
{
  return m_valid;
}

void PeakFinderResults::validate()
{
  m_valid = true;
}
