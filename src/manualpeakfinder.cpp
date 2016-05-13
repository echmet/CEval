#include "manualpeakfinder.h"

ManualPeakFinder::Parameters::Parameters(const QVector<QPointF> &data) :
  data(data)
{
}

std::shared_ptr<PeakFinderResults> ManualPeakFinder::findInternal(const AbstractParameters &ap) throw (std::bad_cast, std::bad_alloc)
{
  const Parameters &p = dynamic_cast<const Parameters&>(ap);
  int fromIndex;
  int toIndex;
  double fromX;
  double fromY;
  double toX;
  double toY;
  std::shared_ptr<PeakFinderResults> r;

  r = std::make_shared<PeakFinderResults>();

  if (p.data.length() < 1)
    return r;

  /* Convert time values to indices */
  {
    int ctr = 0;
    while (ctr < p.data.length()) {
      const double vx = p.data.at(ctr).x();

      if (vx >= p.fromX) {
        fromIndex = ctr;
        break;
      }
      ctr++;
    }
    if (ctr >= p.data.length())
      return r;

    if (p.toX < p.fromX)
      ctr = 0;

    while (ctr < p.data.length()) {
      const double vx = p.data.at(ctr).x();

      if (vx >= p.toX) {
        toIndex = ctr;
        break;
      }
      ctr++;
    }
    if (ctr >= p.data.length()) {
      if (p.toX > p.fromX)
        toIndex = p.data.length() - 1;
      else
        toIndex = 0;
    }
  }

  if (toIndex < fromIndex) {
    r->fromIndex = toIndex;
    r->toIndex = fromIndex;
    fromX = p.toX;
    fromY = p.toY;
    toX = p.fromX;
    toY = p.fromY;
  } else {
    r->fromIndex = fromIndex;
    r->toIndex = toIndex;
    fromX = p.fromX;
    fromY = p.fromY;
    toX = p.toX;
    toY = p.toY;
  }

  if (p.data.first().x() > fromX || p.data.last().x() < toX)
    return r;

  r->peakFromX = fromX;
  r->peakFromY = fromY;
  r->peakToX = toX;
  r->peakToY = toY;
  r->validate();

  return r;
}
