#ifndef HVLEXTRAPOLATOR_H
#define HVLEXTRAPOLATOR_H

#include "peakevaluator.h"
#include <QPointF>
#include <QVector>

class HVLExtrapolator
{
public:
  class Result {
  public:
    double variance;
    double sigma;
    double mean;
  };

  HVLExtrapolator() = delete;

  static QVector<QPointF> extrapolate(const double baselineSlope, const double baselineIntercept,
                                      const double peakHeight,
                                      const double xStep,
                                      const double a0, const double a1, const double a2, const double a3, const QVector<QPointF> &hvlPlot,
                                      const int hvlPrecision, const double tolerance = 1.0);

  static Result varianceFromExtrapolated(const double baselineSlope, const double baselineIntercept,
                                         const double a0,
                                         const QVector<QPointF> &plot);

};

#endif // HVLEXTRAPOLATOR_H
