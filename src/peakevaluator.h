#ifndef PEAKEVALUATOR_H
#define PEAKEVALUATOR_H

#include <cmath>
#include <QVector>
#include <QPointF>

class PeakEvaluator
{
public:
  class Parameters {
  public:
    explicit Parameters() = delete;
    explicit Parameters(const QVector<QPointF> &data);

    int fromIndex;
    int toIndex;

    double fromX;
    double fromY;
    double toX;
    double toY;

    double capillary;
    double detector;
    double E;
    double tEOF;
    double voltage;

    bool noEof;

    const QVector<QPointF> &data;
  };

  class Results {
  public:
    explicit Results();
    bool isValid() const;
    bool isHvlValid() const;
    void validate();
    void validateHvl();

    /*double E;*/
    double uEOF;
    double vEOF;
    double uP;
    double uP_Eff;
    double vP;
    double vP_Eff;

    double minY;
    double maxY;

    double baselineSlope;
    double baselineIntercept;

    int peakIndex;
    double peakX;

    double peakHeight;
    double peakHeightBaseline;

    double peakArea;
    double correctedPeakArea;
    QVector<QPointF> baselineCorrectedPeak;

    double widthHalfLeft;
    double widthHalfRight;
    double widthHalfFull;
    double widthHalfMLeft;
    double widthHalfMRight;
    double widthHalfMFull;

    double sigmaHalfLeft;
    double sigmaHalfRight;
    double sigmaHalfFull;
    double sigmaHalfMLeft;
    double sigmaHalfMRight;
    double sigmaHalfMFull;

    double nLeft;
    double nRight;
    double nFull;
    double nHLeft;
    double nHRight;
    double nHFull;

    double meanX;
    double varianceApex;
    double varianceMean;
    double sigmaApex;
    double sigmaMean;
    double nAsym;

    double HVL_width005;
    double HVL_width005Left;
    double HVL_width005Right;
    double HVL_tP;
    double HVL_tUSP;
    double HVL_migT;
    double HVL_width;
    double HVL_widthLeft;
    double HVL_widthRight;
    double HVL_a0;
    double HVL_a1;
    double HVL_a2;
    double HVL_a3;
    QVector<QPointF> seriesAOne;
    QVector<QPointF> seriesATwo;
    QVector<QPointF> seriesBOne;
    QVector<QPointF> seriesBTwo;

  private:
    bool m_isHvlValid;
    bool m_isValid;
  };

  PeakEvaluator() = delete;

  static Results evaluate(const Parameters &p);
  static Results estimateHvl(const Results &ir, const Parameters &p);

private:
  static void calculateVariances(Results &r);

  static const double Sqrt1_8log2;

};

#endif // PEAKEVALUATOR_H
