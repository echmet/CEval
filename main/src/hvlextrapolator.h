#ifndef HVLEXTRAPOLATOR_H
#define HVLEXTRAPOLATOR_H

#include "hvllibwrapper.h"
#include "peakevaluator.h"
#include <QPointF>
#include <QVector>

class HVLExtrapolator
{
public:
  HVLExtrapolator() = delete;

  static QVector<QPointF> extrapolate(const double baselineSlope, const double baselineIntercept,
                                      const double peakHeight,
                                      const double xStep,
                                      const double a0, const double a1, const double a2, const double a3, const QVector<QPointF> &hvlPlot,
                                      const int hvlPrecision, const double tolerance = 1.0);


};

#endif // HVLEXTRAPOLATOR_H
