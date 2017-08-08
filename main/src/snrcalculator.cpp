#include "snrcalculator.h"
#include <cmath>
#include <QVector>

#include <QDebug>

void calculateBaselineParameters(double &k, double &q, int &fromIdx, int &toIdx, const QVector<QPointF> &data, const QPointF &from, const QPointF &to)
{
  int idx;
  int N;
  double avgX = 0.0;
  double avgY = 0.0;
  double lrNum = 0.0;
  double lrDenom = 0.0;

  for (idx = 0; idx < data.size(); idx++) {
    if (data.at(idx).x() >= from.x()) {
      fromIdx = idx;
      break;
    }
  }
  if (idx == data.size())
    throw std::runtime_error(QObject::tr("Cannot find beginning of the baseline in data").toUtf8());

  for (;idx < data.size(); idx++) {
    if (data.at(idx).x() >= to.x()) {
      toIdx = idx;
      break;
    }
  }
  if (idx == data.size())
    throw std::runtime_error(QObject::tr("Cannot reach the end of the baseline data").toUtf8());

  N = toIdx - fromIdx + 1;

  for (idx = fromIdx; idx <= toIdx; idx++) {
    const QPointF &pt = data.at(idx);

    avgX += pt.x();
    avgY += pt.y();
  }
  avgX /= static_cast<double>(N);
  avgY /= static_cast<double>(N);

  qDebug() << "avgX" << avgX << "avgY" << avgY;

  for (idx = fromIdx; idx <= toIdx; idx++) {
    const QPointF &pt = data.at(idx);

    lrNum += (pt.x() - avgX) * (pt.y() - avgY);
    lrDenom += std::pow(pt.x() - avgX, 2);
  }


  k = lrNum / lrDenom;
  q = avgY - k * avgX;
}

double calculateBaselineStdError(const QVector<QPointF> &data, const int fromIdx, const int toIdx, const double k, const double q)
{
  int dataCtr = 0;
  double stdErr = 0.0;

  for (int idx = fromIdx;idx <= toIdx; idx++) {
    const QPointF &pt = data.at(idx);

    const double linY = k * pt.x() + q;
    stdErr = std::pow(pt.y() - linY, 2);
    dataCtr++;
  }

  return std::sqrt(stdErr / (dataCtr - 2));
}

SNRCalculator::Results SNRCalculator::calculate(const QVector<QPointF> &data, const QPointF &from, const QPointF &to, const double signalMagnitude, const double stdErrAmplifier)
{
  double k;
  double q;
  int fromIdx;
  int toIdx;
  double stdErr;
  double snr;

  if (stdErrAmplifier <= 0.0)
    throw std::runtime_error(QObject::tr("Invalid value of standard error amplifier").toUtf8());

  calculateBaselineParameters(k ,q, fromIdx, toIdx, data, from, to);
  stdErr = calculateBaselineStdError(data, fromIdx, toIdx, k, q);
  snr = signalMagnitude / (stdErr * stdErrAmplifier);

  qDebug() << "k=" << k << "q=" << q;
  qDebug() << "Baseline StdErr" << stdErr << "SNR" << snr;

  const QPointF fromPt(data.at(fromIdx).x(), k * data.at(fromIdx).x() + q);
  const QPointF toPt(data.at(toIdx).x(), k * data.at(toIdx).x() + q);

  return Results{fromPt, toPt, stdErr, snr};
}
