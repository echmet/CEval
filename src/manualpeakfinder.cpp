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
  std::shared_ptr<PeakFinderResults> r;

  r = std::make_shared<PeakFinderResults>();

  /* Convert time values to indices */
  {
    long ctr = 0;
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

    while (ctr < p.data.length()) {
      const double vx = p.data.at(ctr).x();

      if (vx >= p.toX) {
        toIndex = ctr;
        break;
      }
      ctr++;
    }
    if (ctr >= p.data.length())
      toIndex = p.data.length() - 1;
  }


  r->fromIndex = fromIndex;
  r->toIndex = toIndex;
  r->peakFromX = p.fromX; /* Copy the input value because we are in manual mode */
  r->peakFromY = p.fromY; /* Copy the input value because we are in manual mode */
  r->peakToX = p.toX; /* Copy the input value because we are in manual mode */
  r->peakToY = p.toY; /* Copy the input value because we are in manual mode */
  r->validate();

  return r;
}
