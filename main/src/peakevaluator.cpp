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

PeakEvaluator::Results::Results() :
  m_isHvlValid(false),
  m_isValid(false)
{
}

bool PeakEvaluator::Results::isHvlValid() const
{
  return m_isHvlValid;
}

bool PeakEvaluator::Results::isValid() const
{
  return m_isValid;
}

void PeakEvaluator::Results::validate()
{
  m_isValid = true;
}

void PeakEvaluator::Results::validateHvl()
{
  m_isHvlValid = true;
}

void PeakEvaluator::calculateArea(Results &r, const Parameters &p)
{
  /* Subtracting baseline */
  r.baselineCorrectedPeak.reserve(p.toIndex - p.fromIndex + 1);
  for (int i = p.fromIndex; i <= p.toIndex; i++)
    r.baselineCorrectedPeak.push_back(QPointF(p.data.at(i).x(), p.data[i].y() - (r.baselineSlope * p.data[i].x() + r.baselineIntercept)));

  r.peakArea = 0.0;
  for (int idx = 1; idx < r.baselineCorrectedPeak.size(); idx++)
    r.peakArea += ((r.baselineCorrectedPeak.at(idx).y() + r.baselineCorrectedPeak.at(idx - 1).y()) / 2.0) * (p.data.at(idx + p.fromIndex).x() - p.data.at(idx - 1 + p.fromIndex).x());
}

void PeakEvaluator::calculateVariances(Results &r, const Parameters &p)
{
  auto variance = [&r, &p](const double moment) {
    double result = 0.0;

    for (int idx = 1; idx < r.baselineCorrectedPeak.size(); idx++) {
      const QPointF &dataPt = r.baselineCorrectedPeak.at(idx);
      const double dx = dataPt.x() - r.baselineCorrectedPeak.at(idx - 1).x();

      result += dataPt.y() * std::pow(dataPt.x() - moment, 2) * dx;
    }

    return result / r.peakArea;
  };

  const double centroid = [&r, &p] {
    double result = 0.0;

    for (int idx = 1; idx < r.baselineCorrectedPeak.size(); idx++) {
      const QPointF &dataPt = r.baselineCorrectedPeak.at(idx);
      const double dx = dataPt.x() - r.baselineCorrectedPeak.at(idx - 1).x();

      result += dataPt.y() * dataPt.x() * dx;
    }

    return result / r.peakArea;
  }();

  r.varianceApex = variance(r.peakX);
  r.varianceCentroid = variance(centroid);
  r.sigmaApex = std::sqrt(r.varianceApex);
  r.sigmaCentroid = std::sqrt(r.varianceCentroid);
}


PeakEvaluator::Results PeakEvaluator::estimateHvl(const Results &ir, const Parameters &p)
{
  static auto isInputValid = [](const Parameters &p) {
    return p.data.length() > 1 && p.fromIndex >= 0 && p.toIndex > p.fromIndex && p.toIndex < p.data.length();
  };
  const QVector<QPointF> &Data = p.data;
  Results r = ir;

  if (!isInputValid(p)) {
    QMessageBox::warning(nullptr, QObject::tr("Evaluation error"), QString(QObject::tr("Some of the evaluation parameters are not valid, peak area and HVL estimation values"
                                                                                       " cannot be calculated\n\n"
                                                                                       "Data length: %1, tAi: %2, tBi: %3")).arg(Data.length()).arg(p.fromIndex).arg(p.toIndex));
    return r;
  }

  /* Height (5 %) */
  int i_w005, j_w005;

  /* TODO: Peak base search should probably take the baseline noise into account */
  const int centerIndex = ir.peakIndex - p.fromIndex;

  for (i_w005 = centerIndex; i_w005 > 0 && std::abs(r.baselineCorrectedPeak.at(i_w005).y()) >= std::abs(0.05 * r.peakHeightBaseline); --i_w005);

  double width005Left = r.peakX - Data.at(i_w005 + p.fromIndex).x();

  for (j_w005 = centerIndex; j_w005 < r.baselineCorrectedPeak.size() && std::abs(r.baselineCorrectedPeak.at(j_w005).y()) >= std::abs(0.05 * r.peakHeightBaseline); ++j_w005);

  double width005Right = Data.at(j_w005 + p.fromIndex).x() - r.peakX;

  r.HVL_width005 = width005Left + width005Right;
  r.HVL_width005Left = width005Left;
  r.HVL_width005Right = width005Right;

  r.seriesAOne.push_back(QPointF(Data[i_w005 + p.fromIndex].x(), r.baselineSlope * Data[i_w005].x() + r.baselineIntercept));
  r.seriesATwo.push_back(QPointF(Data[i_w005 + p.fromIndex].x(), r.peakHeightBaseline));
  r.seriesBOne.push_back(QPointF(Data[i_w005 + p.fromIndex].x(), r.peakHeightBaseline * Data[i_w005].x() + r.baselineIntercept));
  r.seriesBTwo.push_back(QPointF(Data[i_w005 + p.fromIndex].x(), r.peakHeightBaseline));

  /* HVL Estimation */
  r.HVL_tP = r.peakX;
  r.HVL_tUSP = (width005Right + width005Left) / (2 * width005Left);

  r.HVL_migT = r.HVL_tP;
  r.HVL_width = r.widthHalfFull;
  r.HVL_widthLeft = r.widthHalfLeft;
  r.HVL_widthRight = r.widthHalfRight;

  echmet::HVLCore::Coefficients HVL_coefficients = echmet::HVLCore::Coefficients::Calculate(r.peakArea, r.HVL_tP, r.widthHalfFull, r.HVL_tUSP);

  r.HVL_a1 = HVL_coefficients.a1;
  r.HVL_a2 = HVL_coefficients.a2;
  r.HVL_a3 = HVL_coefficients.a3d;

  r.HVL_tP = r.HVL_a1;
  r.validateHvl();

  return r;
}

PeakEvaluator::Results PeakEvaluator::evaluate(const PeakEvaluator::Parameters &p)
{
  Results r;
  double Detector = p.detector * 1.0E-2; /* Conversion from centimeters */
  double E;
  int twPLefti;
  double twPLeft;
  int twPRighti;
  double twPRight;

  if (p.toIndex - p.fromIndex < 1)
    return r;

  r.minY = Helpers::minYValue(p.data);
  r.maxY = Helpers::maxYValue(p.data);

  /* -- Calculated results -- */
  /* --> System */
  double Capillary = p.capillary * 1.0E-2; /* Conversion from centimeters */
  double Voltage = p.voltage * 1.0E+3; /* Conversion from kilovolts */

  r.baselineSlope = (p.toY - p.fromY) / (p.toX - p.fromX);
  r.baselineIntercept = p.fromY - p.fromX * r.baselineSlope;

  auto heightAt = [&](const int idx) {
    return p.data.at(idx).y() - r.baselineSlope * p.data.at(idx).x() - r.baselineIntercept;
  };

  {
    /* Find the extreme. The extreme can be either above or below
    * the baseline, only the absolute value matters */
    double maxDiff = std::abs(heightAt(p.fromIndex));
    r.peakIndex = p.fromIndex;
    r.peakX = p.data.at(p.fromIndex).x();
    for (int idx = p.fromIndex; idx <= p.toIndex; idx++) {
      const double diff = std::abs(heightAt(idx));

      if (diff > maxDiff) {
        r.peakIndex = idx;
        r.peakX = p.data.at(idx).x();
        maxDiff = diff;
      }
    }
  }

  r.peakHeightBaseline = heightAt(r.peakIndex);
  r.peakHeight = p.data.at(r.peakIndex).y();

  /* Width of peak at half of its height */
  twPLefti = r.peakIndex;
  do {
    twPLefti--;
    twPLeft = heightAt(twPLefti);
  } while ((twPLefti > p.fromIndex) && (std::abs(r.peakHeightBaseline) / 2.0 < std::abs(twPLeft)));

  twPLeft = p.data.at(twPLefti).x();

  twPRighti = r.peakIndex;
  do {
    twPRighti++;
    twPRight = heightAt(twPRighti);
  } while ((twPRighti < p.toIndex) && std::abs(r.peakHeightBaseline) / 2.0 < std::abs(twPRight));

  twPRight = p.data.at(twPRighti).x();

  if (p.capillary > 0.0)
    E = Voltage / Capillary;
  else {
    if (Voltage == 0.0)
      E = 0;
    else
      E = std::numeric_limits<double>::infinity();
  }

  /* EOF */
  if (p.noEof)
    r.vEOF = 0.0;
  else {
    if (p.tEOF > 0.0)
      r.vEOF = Detector / (p.tEOF * 60.0);
    else {
      if (E == 0.0)
        r.vEOF = 0.0;
      else
        r.vEOF = std::numeric_limits<double>::infinity();
    }
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
  if (r.peakX > 0.0)
    r.vP = Detector / (r.peakX * 60.0);
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


  r.widthHalfLeft = r.peakX - twPLeft;
  r.widthHalfRight = twPRight - r.peakX;

  r.widthHalfFull = r.widthHalfLeft + r.widthHalfRight;

  r.sigmaHalfLeft = 2.0 * r.widthHalfLeft * Sqrt1_8log2;
  r.sigmaHalfRight = 2.0 * r.widthHalfRight * Sqrt1_8log2;
  r.sigmaHalfFull = r.widthHalfFull * Sqrt1_8log2;

  if (Helpers::isSensible(r.vP)) {
    r.widthHalfMLeft = r.widthHalfLeft * r.vP;
    r.widthHalfMRight = r.widthHalfRight * r.vP;
    r.widthHalfMFull = r.widthHalfFull * r.vP;

    r.sigmaHalfMLeft = r.sigmaHalfLeft * r.vP;
    r.sigmaHalfMRight = r.sigmaHalfRight * r.vP;
    r.sigmaHalfMFull = r.sigmaHalfFull * r.vP;
  } else {
    r.widthHalfMLeft = 0.0;
    r.widthHalfMRight = 0.0;
    r.widthHalfMFull = 0.0;

    r.sigmaHalfMLeft = 0.0;
    r.sigmaHalfMRight = 0.0;
    r.sigmaHalfMFull = 0.0;
  }

  if (Helpers::isSensible(r.sigmaHalfLeft)) {
    r.nLeft = r.peakX / r.sigmaHalfLeft;
    r.nLeft *= r.nLeft;
  } else
    r.nLeft = 0.0;
  if (Helpers::isSensible(r.sigmaHalfRight)) {
    r.nRight = r.peakX / r.sigmaHalfRight;
    r.nRight *= r.nRight;
  } else
    r.nRight = 0.0;
  if (Helpers::isSensible(r.sigmaHalfFull)) {
    r.nFull = r.peakX / r.sigmaHalfFull;
    r.nFull *= r.nFull;
  } else
    r.nFull = 0.0;

  if (Helpers::isSensible(r.nLeft))
    r.nHLeft = Detector / r.nLeft;
  else
    r.nHLeft = 0.0;

  if (Helpers::isSensible(r.nRight))
    r.nHRight = Detector / r.nRight;
  else
    r.nHRight = 0.0;

  if (Helpers::isSensible(r.nFull))
    r.nHFull = Detector / r.nFull;
  else
    r.nHFull = 0.0;

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

  calculateArea(r, p);
  calculateVariances(r, p);

  r.validate();

  return r;
}
