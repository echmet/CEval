#include "manualpeakfinder.h"

ManualPeakFinder::Parameters::Parameters(const QVector<QPointF> &data) :
  data(data)
{
}

ManualPeakFinder::Parameters::~Parameters()
{
}

std::shared_ptr<PeakFinderResults> ManualPeakFinder::findInternal(const AbstractParameters &ap) noexcept(false)
{
  const Parameters &p = dynamic_cast<const Parameters&>(ap);
  const int length = p.data.length();
  int fromIndex;
  int toIndex;
  double fromX;
  double fromY;
  double toX;
  double toY;
  QVector<std::shared_ptr<PeakFinderResults::Result>> results;

  if (length < 1)
    return std::shared_ptr<PeakFinderResults>(new PeakFinderResults());

  /* Convert time values to indices */
  {
    int ctr = 0;
    while (ctr < length) {
      const double vx = p.data.at(ctr).x();

      if (vx >= p.fromX) {
        fromIndex = ctr;
        break;
      }
      ctr++;
    }
    if (ctr >= length)
      return std::shared_ptr<PeakFinderResults>(new PeakFinderResults());

    if (p.toX < p.fromX)
      ctr = 0;

    while (ctr < length) {
      const double vx = p.data.at(ctr).x();

      if (vx >= p.toX) {
        toIndex = ctr;
        break;
      }
      ctr++;
    }
    if (ctr >= length) {
      if (p.toX > p.fromX)
        toIndex = length - 1;
      else
        toIndex = 0;
    }
  }

  if (toIndex < fromIndex) {
    std::swap(fromIndex, toIndex);
    fromX = p.toX;
    fromY = p.toY;
    toX = p.fromX;
    toY = p.fromY;
  } else {
    fromX = p.fromX;
    fromY = p.fromY;
    toX = p.toX;
    toY = p.toY;
  }

  if (p.data.first().x() > fromX || p.data.last().x() < toX)
    return std::shared_ptr<PeakFinderResults>(new PeakFinderResults());

  results.push_back(std::shared_ptr<PeakFinderResults::Result>(new PeakFinderResults::Result(fromIndex, toIndex,
                                                                                             fromX, fromY,
                                                                                             toX, toY)));

  return std::shared_ptr<PeakFinderResults>(new PeakFinderResults(results));
}
