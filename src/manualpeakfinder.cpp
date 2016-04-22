#include "manualpeakfinder.h"
#include "helpers.h"
#include <functional>

ManualPeakFinder::Parameters::Parameters(const QVector<QPointF> &data) :
  data(data)
{
}

PeakFinderResults ManualPeakFinder::findInternal(const AbstractParameters &ap) throw (std::bad_cast)
{
  const Parameters &p = dynamic_cast<const Parameters&>(ap);
  double baselineSlope;
  double baselineIntercept;
  double peakHeightBaseline;
  long tPi;
  long fromIndex;
  long toIndex;
  long twPLefti;
  double twPLeft;
  long twPRighti;
  double twPRight;
  double peakX;
  PeakFinderResults r;
  std::function<double(const long idx)> heightAt;

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
      return PeakFinderResults();

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


  baselineSlope = (p.toY - p.fromY) / (p.toX - p.fromX);
  baselineIntercept = p.fromY - p.fromX * baselineSlope;


  if (p.valley) {
    heightAt = [&p, baselineSlope, baselineIntercept](const long idx) {
      return baselineSlope * p.data.at(idx).x() - baselineIntercept - p.data.at(idx).y();
    };
  } else {
    heightAt = [&p, baselineSlope, baselineIntercept](const long idx) {
      return p.data.at(idx).y() - baselineSlope * p.data.at(idx).x() - baselineIntercept;
    };
  }

  {
    /* Find the extreme. The extreme can be either above or below
    * the baseline, only the absolute value matters */
    double maxDiff = heightAt(fromIndex);
    tPi = fromIndex;
    peakX = p.data.at(fromIndex).x();
    for (long idx = fromIndex; idx <= toIndex; idx++) {
      const double diff = heightAt(idx);

      if (diff > maxDiff) {
        tPi = idx;
        peakX = p.data.at(idx).x();
        maxDiff = diff;
      }
    }
  }

  peakHeightBaseline = heightAt(tPi);

  /* Width of peak at half of its height */
  twPLefti = tPi;
  do {
    twPLefti--;
    twPLeft = heightAt(twPLefti);
  } while ((twPLefti > fromIndex) && (std::abs(peakHeightBaseline) / 2.0 < twPLeft));

  twPLeft = p.data.at(twPLefti).x();

  twPRighti = tPi;
  do {
    twPRighti++;
    twPRight = heightAt(twPRighti);
  } while (twPRighti < toIndex && std::abs(peakHeightBaseline) / 2.0 < twPRight);

  twPRight = p.data.at(twPRighti).x();


  r.baselineIntercept = baselineIntercept;
  r.baselineSlope = baselineSlope;
  r.fromIndex = 0; /* This value makes no sense in manual mode */
  r.fromPeakIndex = fromIndex;
  r.indexAtMax = tPi;
  r.maxY = Helpers::maxYValue(p.data);
  r.minY = Helpers::minYValue(p.data);
  r.noise = 0.0; /* This value makes no sense in manual mode */
  r.noiseRefPoint = 0.0; /* This value makes no sense in manual mode */
  r.peakFromX = p.fromX; /* Copy the input value because we are in manual mode */
  r.peakFromY = p.fromY; /* Copy the input value because we are in manual mode */
  r.peakHeight = p.data.at(tPi).y();
  r.peakHeightBaseline = peakHeightBaseline;
  r.peakToX = p.toX; /* Copy the input value because we are in manual mode */
  r.peakToY = p.toY; /* Copy the input value because we are in manual mode */
  r.peakX = peakX;
  r.seriesA = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>()); /* We're not gonna take it [manual mode] */
  r.seriesB = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>()); /* We don't want nothing, not a thing from you [manual mode] */
  r.slopeRefPoint = 0.0; /* This value makes no sense in manual mode */
  r.slopeThreshold = 0.0; /* This value makes no sense in manual mode */
  r.slopeWindow = 0.0; /* This value makes no sense in manual mode */
  r.toIndex = p.data.length() - 1; /* This value makes no sense in manual mode */
  r.toPeakIndex = toIndex;
  r.tPiCoarse = tPi; /* This value makes no sense in manual mode */
  r.twPLeft = twPLeft;
  r.twPRight = twPRight;
  r.validate();


  return r;
}
