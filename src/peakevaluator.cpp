#include "peakevaluator.h"
#include "helpers.h"
#include "math/functions.h"
#include "math/hvlestimate.h"
#include <limits>
#include <QMessageBox>
#include <QVector>

const double PeakEvaluator::Sqrt1_8log2 = sqrt(1.0 / (8.0 * log(2.0)));

PeakEvaluator::Parameters::Parameters(const QVector<QPointF> &data) :
  data(data)
{
}


PeakEvaluator::Results PeakEvaluator::evaluate(const PeakEvaluator::Parameters &p)
{
  Results r;
  const QVector<QPointF> &Data = p.data;
  double Detector = p.detector * 1.0E-2; /* Conversion from centimeters */
  double E;

  /* -- Calculated results -- */
  /* --> System */
  double Capillary = p.capillary * 1.0E-2; /* Conversion from centimeters */
  double Voltage = p.voltage * 1.0E+3; /* Conversion from kilovolts */

  if (p.capillary > 0.0)
    E = Voltage / Capillary;
  else {
    if (Voltage == 0.0)
      E = 0;
    else
      E = std::numeric_limits<double>::infinity();
  }

  /* EOF */
  if (p.tEOF > 0.0)
    r.vEOF = Detector / (p.tEOF * 60.0);
  else {
    if (E == 0.0)
      r.vEOF = 0.0;
    else
      r.vEOF = std::numeric_limits<double>::infinity();
  }

  if (std::isfinite(E) && std::isfinite(r.vEOF))
    r.uEOF = (r.vEOF / E);
  else {
    if (E == 0.0)
      r.uEOF = 0.0;
    else
      r.uEOF = std::numeric_limits<double>::infinity();
  }

  /* Peak */
  if (p.tP > 0.0)
    r.vP = Detector / (p.tP * 60.0);
  else
    r.vP = std::numeric_limits<double>::infinity();

  if (std::isfinite(r.vP) && std::isfinite(E))
    r.uP = r.vP / E;
  else
    r.uP = std::numeric_limits<double>::infinity();

  if (std::isfinite(r.vEOF) && std::isfinite(r.vP))
    r.vP_Eff = r.vP - r.vEOF;
  else
    r.vP_Eff = std::numeric_limits<double>::infinity();

  if (std::isfinite(E) && std::isfinite(r.vP_Eff))
    r.uP_Eff = r.vP_Eff / E;

  if (p.autoWidthHalfLeft) {
    r.widthHalfLeft = p.tP - p.tWPLeft;
    r.widthHalfRight = p.tWPRight - p.tP;
  } else
    r.widthHalfLeft = r.widthHalfRight = p.widthHalfLeft;

  r.widthHalfFull = r.widthHalfLeft + r.widthHalfRight;

  r.sigmaHalfLeft = 2.0 * r.widthHalfLeft * Sqrt1_8log2;
  r.sigmaHalfRight = 2.0 * r.widthHalfRight * Sqrt1_8log2;
  r.sigmaHalfFull = r.widthHalfFull * Sqrt1_8log2;

  if (Helpers::isSensible(r.vP)) {
    r.widthHalfMLeft = r.widthHalfLeft * r.vP;
    r.widthHalfMRight = r.widthHalfMRight * r.vP;
    r.widthHalfMFull = r.widthHalfFull * r.vP;

    r.sigmaHalfMLeft = r.sigmaHalfLeft * r.vP;
    r.sigmaHalfMRight = r.sigmaHalfRight * r.vP;
    r.sigmaHalfMFull = r.sigmaHalfFull * r.vP;
  }

  if (Helpers::isSensible(r.sigmaHalfLeft)) {
    r.nLeft = p.tP / r.sigmaHalfLeft;
    r.nLeft *= r.nLeft;
  }
  if (Helpers::isSensible(r.sigmaHalfRight)) {
    r.nRight = p.tP / r.sigmaHalfRight;
    r.nRight *= r.nRight;
  }
  if (Helpers::isSensible(r.sigmaHalfFull)) {
    r.nFull = p.tP / r.sigmaHalfFull;
    r.nFull *= r.nFull;
  }

  if (Helpers::isSensible(r.nLeft))
    r.nHLeft = Detector / r.nLeft;

  if (Helpers::isSensible(r.nRight))
    r.nHRight = Detector / r.nRight;

  if (Helpers::isSensible(r.nFull))
    r.nHFull = Detector / r.nFull;


  /* Subtracting baseline */
  if (Data.length() > 1 && p.tAi >= 0 && p.tBi > p.tAi && p.tBi < Data.length()) {
    QVector<double> peak_subtracted;

    peak_subtracted.reserve(p.tBi - p.tAi);
    for (long i = p.tAi; i <= p.tBi; i++)
      peak_subtracted.push_back(Data[i].y() - (p.BSLSlope * Data[i].x() + p.BSLIntercept));

     r.peakArea = 0.0;

     /* Peak::Peak Area */
     for (
          QVector<double>::iterator i = peak_subtracted.begin(),
          i_end = peak_subtracted.end()
          ;
          i != i_end
          ;
          ++i
         ) r.peakArea += *i;

    r.peakArea -= (peak_subtracted[0] + peak_subtracted.back()) / 2;
    r.peakArea *= (Data[1].x() - Data[0].x());

    /* Height (5 %) */
    int i_w005, j_w005;

    for (i_w005 = 0; i_w005 < peak_subtracted.size() && std::fabs(peak_subtracted[i_w005]) <= std::fabs(0.05 * p.HP_BSL); ++i_w005);

    double width005Left = p.tP - Data[i_w005 + p.tAi].x();

    for (j_w005 = (peak_subtracted.size() - 1); j_w005 > i_w005 && std::fabs(peak_subtracted[j_w005]) <= std::fabs(0.05 * p.HP_BSL); --j_w005);

    double width005Right = Data[j_w005 + p.tAi].x() - p.tP;

    r.HVL_width005 = width005Left + width005Right;
    r.HVL_width005Left = width005Left;
    r.HVL_width005Right = width005Right;


    r.seriesAOne.push_back(QPointF(Data[i_w005 + p.tAi].x(), p.BSLSlope * Data[i_w005].x() + p.BSLIntercept));
    r.seriesATwo.push_back(QPointF(Data[i_w005 + p.tAi].x(), p.HP_BSL));
    r.seriesBOne.push_back(QPointF(Data[i_w005 + p.tAi].x(), p.BSLSlope * Data[i_w005].x() + p.BSLIntercept));
    r.seriesBTwo.push_back(QPointF(Data[i_w005 + p.tAi].x(), p.HP_BSL));

    /*R.SRS1A = XYPair(Data[i_w005 + P.TAi].X, P.BSLSlope * Data[i_w005].X + P.BSLIntercept);
    R.SRS1B = XYPair(Data[i_w005 + P.TAi].X, P.HP_BSL);
    R.SRS2A = XYPair(Data[i_w005 + P.TAi].X, P.BSLSlope * Data[i_w005].X + P.BSLIntercept);
    R.SRS2B = XYPair(Data[i_w005 + P.TAi].X, P.HP_BSL);*/

    /* HVL Estimation */
    r.HVL_tP = p.tP;
    r.HVL_tUSP = (width005Right + width005Left) / (2 * width005Left);

    r.HVL_migT = r.HVL_tP;
    r.HVL_width = r.widthHalfFull;
    r.HVL_widthLeft = r.widthHalfLeft;
    r.HVL_widthRight = r.widthHalfRight;

    echmet::HVLCore::Coefficients HVL_coefficients(
        r.peakArea,
        r.HVL_tP,
        r.widthHalfFull,
        r.HVL_tUSP,
        int()
    );


    r.HVL_a1 = HVL_coefficients.a1;
    r.HVL_a2 = HVL_coefficients.a2;
    r.HVL_a3 = HVL_coefficients.a3d;

    r.HVL_tP = r.HVL_a1;

  } else {
    QMessageBox::warning(nullptr, QObject::tr("Evaluation error"), QString(QObject::tr("Some of the evaluation parameters are not valid, peak area and HVL estimation values"
                                                                                       " cannot be calculated\n\n"
                                                                                       "Data length: %1, tAi: %2, tBi: %3")).arg(Data.length()).arg(p.tAi).arg(p.tBi));
  }

  /* Convert velocities and mobilities to mode readable values */
  if (std::isfinite(r.uEOF))
    r.uEOF /= 1.0e-9;
  if (std::isfinite(r.uP))
    r.uP /= 1.0e-9;
  if (std::isfinite(r.uP_Eff))
    r.uP_Eff /= 1.0e-9;

  if (std::isfinite(r.vEOF))
    r.vEOF /= 1.0e-3;
  if (std::isfinite(r.vP))
    r.vP /= 1.0e-3;
  if (std::isfinite(r.vP_Eff))
    r.vP_Eff /= 1.0e-3;


  return r;
}
