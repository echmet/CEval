#include "hvlextrapolator.h"

QVector<QPointF> HVLExtrapolator::extrapolate(const double baselineSlope, const double baselineIntercept,
                                              const double peakHeight,
                                              const double xStep,
                                              const double a0, const double a1, const double a2, const double a3, const QVector<QPointF> &hvlPlot,
                                              const int hvlPrecision, const double tolerance)
{
  HVLLibWrapper hvlWrapper(hvlPrecision);
  QVector<QPointF> extrapolatedHvlPlot;

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
