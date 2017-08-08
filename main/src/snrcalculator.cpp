#include "snrcalculator.h"
#include <cmath>
#include <QObject>
#include <QVector>

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

  for (idx = fromIdx; idx <= toIdx; idx++) {
    const QPointF &pt = data.at(idx);

    lrNum += (pt.x() - avgX) * (pt.y() - avgY);
    lrDenom += std::pow(pt.x() - avgX, 2);
  }


  k = lrNum / lrDenom;
  q = avgY - k * avgX;
}

double calculateBaselineSigma(const QVector<QPointF> &data, const int fromIdx, const int toIdx, const double k, const double q)
{
  double sigma = 0.0;

  for (int idx = fromIdx;idx <= toIdx; idx++) {
    const QPointF &pt = data.at(idx);

    const double linY = k * pt.x() + q;
    sigma += std::pow(pt.y() - linY, 2);
  }

  return std::sqrt(sigma / (toIdx - fromIdx - 1));
}

SNRCalculator::Results SNRCalculator::calculate(const QVector<QPointF> &data, const QPointF &from, const QPointF &to, const double signalMagnitude, const double sigmaAmplifier)
{
  double k;
  double q;
  int fromIdx;
  int toIdx;
  double sigma;
  double snr;

  if (sigmaAmplifier <= 0.0)
    throw std::runtime_error(QObject::tr("Invalid value of standard error amplifier").toUtf8());

  calculateBaselineParameters(k ,q, fromIdx, toIdx, data, from, to);
  sigma = calculateBaselineSigma(data, fromIdx, toIdx, k, q);
  snr = signalMagnitude / (sigma * sigmaAmplifier);

  const QPointF fromPt(data.at(fromIdx).x(), k * data.at(fromIdx).x() + q);
  const QPointF toPt(data.at(toIdx).x(), k * data.at(toIdx).x() + q);

  return Results{fromPt, toPt, sigma, snr};
}
