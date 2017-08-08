#ifndef SNRCALCULATOR_H
#define SNRCALCULATOR_H

#include <QPointF>


class SNRCalculator
{
public:
  class Results {
  public:
    const QPointF from;
    const QPointF to;
    const double stdErr;
    const double snr;
  };

  SNRCalculator() = delete;

  static Results calculate(const QVector<QPointF> &data, const QPointF &from, const QPointF &to, const double signalMagnitude, const double sigmaAmplifier);
};

#endif // SNRCALCULATOR_H
