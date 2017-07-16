#include "hvlextrapolator.h"
#include <cmath>
#include <functional>

QVector<QPointF> HVLExtrapolator::extrapolate(const double baselineSlope, const double baselineIntercept,
                                              const double peakHeight,
                                              const double xStep,
                                              const double a0, const double a1, const double a2, const double a3, const QVector<QPointF> &hvlPlot,
                                              const int hvlPrecision, const double tolerance)
{
  HVLLibWrapper hvlWrapper(hvlPrecision);
  QVector<QPointF> extrapolatedHvlPlot;

  if (tolerance <= 0.0)
    return QVector<QPointF>();

  const double threshold = std::abs(peakHeight * tolerance / 100.0);

  auto extrapolatingFunc = [&](const double initialX, std::function<double (const double)> xShifter) {
    QVector<QPointF> plot;
    double x = initialX;

    while (true) {
      x = xShifter(x);

      const double y = hvlWrapper.calculate(HVLLibWrapper::Parameter::T, x, a0, a1, a2, a3).y;
      const double baselineY = baselineSlope * x + baselineIntercept;

      if (std::abs(y) <= threshold)
        return plot;

      plot.push_back(QPointF(x, y + baselineY));
    }
  };

  /* Move forward from the beginning of the peak until we drop below baseline tolerace */
  QVector<QPointF> ahead = extrapolatingFunc(hvlPlot.front().x(), [xStep](const double x) { return x - xStep; });
  QVector<QPointF> behind = extrapolatingFunc(hvlPlot.back().x(), [xStep](const double x) { return x + xStep; });

  for (int idx = ahead.size() - 1; idx >= 0; idx--)
    extrapolatedHvlPlot.push_back(ahead.at(idx));

  extrapolatedHvlPlot.append(hvlPlot);
  extrapolatedHvlPlot.append(behind);

  return extrapolatedHvlPlot;
}

HVLExtrapolator::Result HVLExtrapolator::varianceFromExtrapolated(const double baselineSlope, const double baselineIntercept,
                                                 const double a0,
                                                 const QVector<QPointF> &plot)
{
  QVector<QPointF> blCorrPlot = [baselineSlope, baselineIntercept, &plot]() {
    QVector<QPointF> nPlot;

    nPlot.reserve(plot.size());

    for (const QPointF &pt : plot) {
      const double baselineY = baselineSlope * pt.x() + baselineIntercept;

      nPlot.push_back(QPointF(pt.x(), pt.y() - baselineY));
    }

    return nPlot;
  }();

  auto variance = [&blCorrPlot, a0](const double moment) {
    double result = 0.0;

    for (int idx = 1; idx < blCorrPlot.size(); idx++) {
      const QPointF &dataPt = blCorrPlot.at(idx);
      const double dx = dataPt.x() - blCorrPlot.at(idx - 1).x();

      result += dataPt.y() * std::pow(dataPt.x() - moment, 2) * dx;
    }

    return result / a0;
  };

  const double mean = [&blCorrPlot, a0] {
    double result = 0.0;

    for (int idx = 1; idx < blCorrPlot.size(); idx++) {
      const QPointF &dataPt = blCorrPlot.at(idx);
      const double dx = dataPt.x() - blCorrPlot.at(idx - 1).x();

      result += dataPt.y() * dataPt.x() * dx;
    }

    return result / a0;
  }();

  const double var = variance(mean);

  return { var, std::sqrt(var), mean };
}
