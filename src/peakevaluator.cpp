#include "peakevaluator.h"
#include "helpers.h"
#include "math/functions.h"
#include "math/hvlestimate.h"
#include <limits>
#include <tuple>
#include <QMessageBox>
#include <QVector>

QVector<QPointF> correctForBaseline(const int fromIdx, const int toIdx,
                                    const double blK, const double blQ,
                                    const QVector<QPointF> &data)
{
  QVector<QPointF> blCorrected;
  blCorrected.reserve(toIdx - fromIdx + 1);
  for (int idx = fromIdx; idx < toIdx; idx++) {
    const double x = data.at(idx).x();
    const double y = data.at(idx).y();

    blCorrected.push_back(QPointF(x, y - (blK * x + blQ)));
  }

  return blCorrected;
}

double calculateArea(const QVector<QPointF> &data, const bool correctForX)
{
  double peakArea = 0.0;

  for (int idx = 1; idx < data.size(); idx++) {
    const double xL = data.at(idx - 1).x();
    const double yL = data.at(idx - 1).y();
    const double xR = data.at(idx).x();
    const double yR = data.at(idx).y();

    double slice =  ((yL + yR) / 2.0) * (xR - xL);
    if (correctForX) {
      double xAvg = (xR + xR) / 2.0;
      if (xAvg <= 0.0)
        return std::numeric_limits<double>::quiet_NaN();
      slice /= xAvg;
    }
    peakArea += slice;
  }

  return peakArea;
}

const double PeakEvaluator::Sqrt1_8log2 = std::sqrt(1.0 / (8.0 * std::log(2.0)));

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

void PeakEvaluator::calculateVariances(Results &r)
{
  auto variance = [&r](const double moment) {
    double result = 0.0;

    for (int idx = 1; idx < r.baselineCorrectedPeak.size(); idx++) {
      const QPointF &dataPt = r.baselineCorrectedPeak.at(idx);
      const double dx = dataPt.x() - r.baselineCorrectedPeak.at(idx - 1).x();

      result += dataPt.y() * std::pow(dataPt.x() - moment, 2) * dx;
    }

    return result / r.peakArea;
  };

  const double mean = [&r] {
    double result = 0.0;

    for (int idx = 1; idx < r.baselineCorrectedPeak.size(); idx++) {
      const QPointF &dataPt = r.baselineCorrectedPeak.at(idx);
      const double dx = dataPt.x() - r.baselineCorrectedPeak.at(idx - 1).x();

      result += dataPt.y() * dataPt.x() * dx;
    }

    return result / r.peakArea;
  }();

  r.varianceApex = variance(r.peakX);
  r.varianceMean = variance(mean);
  r.sigmaApex = std::sqrt(r.varianceApex);
  r.sigmaMean= std::sqrt(r.varianceMean);
  r.meanX = mean;
}


PeakEvaluator::Results PeakEvaluator::estimateHvl(const Results &ir, const Parameters &p)
{
  static auto isInputValid = [](const Parameters &p) {
    return p.data.length() > 1 && p.fromIndex >= 0 && p.toIndex > p.fromIndex && p.toIndex < p.data.length();
  };

  static auto calculateKQ = [](const QPointF &left, const QPointF &right) {
    const double k = (right.y() - left.y()) / (right.x() - left.x());
    const double q = left.y() - k * left.x();

    return std::make_tuple(k, q);
  };

  static auto extrapolateEdge = [](const double blK, double blQ, const double h,
                                   const QPointF &left, const QPointF &right) {
    const std::tuple<double, double> peakKQ = calculateKQ(left, right);
    const double pkK = std::get<0>(peakKQ);
    const double pkQ = std::get<1>(peakKQ);
    blQ += h * 0.05;

    const double x = (pkQ - blQ) / (blK - pkK);

    return x;
  };

  static auto extrapolateA0 = [](const double fromX, const double toX,
                                 const double blK, const double blQ,
                                 const QVector<QPointF> &data) {
    int fromIdx = -1;
    int toIdx = -1;

    int idx;
    for (idx = 0; idx < data.size(); idx++) {
      const double x = data.at(idx).x();
      if (x >= fromX) {
        fromIdx = idx;
        break;
      }
    }
    for (; idx < data.size(); idx++) {
      const double x = data.at(idx).x();
      if (x >= toX) {
        toIdx = idx;
        break;
      }
    }

    if (fromIdx == -1 || toIdx == -1)
      throw std::runtime_error("Cannot find extrapolated peak boundaries");

    const QVector<QPointF> blCorrected = correctForBaseline(fromIdx, toIdx, blK, blQ, data);
    return calculateArea(blCorrected, false);
  };

  Results r = ir;

  if (!isInputValid(p)) {
    QMessageBox::warning(nullptr, QObject::tr("Evaluation error"), QString(QObject::tr("Some of the evaluation parameters are not valid, peak area and HVL estimation values"
                                                                                       " cannot be calculated\n\n"
                                                                                       "Data length: %1, tAi: %2, tBi: %3")).arg(p.data.length()).arg(p.fromIndex).arg(p.toIndex));
    return r;
  }

  /* Height (5 %) */
  int left_w005 = -1;
  int right_w005 = -1;

  /* TODO: Peak base search should probably take the baseline noise into account */
  const auto isAtBorder = [&ir](const double yVal) {
    return std::abs(yVal) <= std::abs(0.05 * ir.peakHeightBaseline);
  };

  const auto clampIndex = [&](const int idx) -> intptr_t {
    if (idx < 0)
      return 0;
    else if (idx >= p.data.size() - 1)
      return p.data.size() - 1;
    return idx;
  };

  const auto maximumIndex = [&]() -> intptr_t {
    auto mi = ir.peakIndex - p.fromIndex;
    if (mi >= r.baselineCorrectedPeak.size())
      return r.baselineCorrectedPeak.size() - 1;
    else if (mi < 0)
      return 0;
    return mi;
  }();
  /* Check that we have an actual peak, meaning that its maximum
   * must not be at neither of the ends of the given signal slice.
     TODO: This should take noise in to account to be effective. */
  if (maximumIndex == 0 || maximumIndex == r.baselineCorrectedPeak.size() - 1) {
    QMessageBox box(QMessageBox::Warning, QObject::tr("HVL evaluation error"), QObject::tr("Selected signal does not resemble a peak"));
    box.exec();

    return r;
  }

  for (int idx = maximumIndex; idx >= 0; idx--) {
    if (isAtBorder(r.baselineCorrectedPeak.at(idx).y())) {
      left_w005 = idx;
      break;
    }
  }

  for (int idx = maximumIndex; idx < r.baselineCorrectedPeak.size(); idx++) {
    if (isAtBorder(r.baselineCorrectedPeak.at(idx).y())) {
      right_w005 = idx;
      break;
    }
  }

  double left005x;
  double right005x;
  bool useExtrapolateA0 = false;

  if (left_w005 == -1) {
    const auto fIdx = clampIndex(p.fromIndex);
    const auto tIdx = clampIndex(maximumIndex + p.fromIndex);
    left005x = extrapolateEdge(ir.baselineSlope, ir.baselineIntercept, ir.peakHeightBaseline,
                               QPointF(p.fromX, p.data.at(fIdx).y()),
                               QPointF(p.data.at(tIdx).x(), p.data.at(tIdx).y()));
    useExtrapolateA0 = true;
  } else
    left005x = p.data.at(left_w005 + p.fromIndex).x();

  if (right_w005 == -1) {
    const auto fIdx = clampIndex(maximumIndex + p.fromIndex);
    const auto tIdx = clampIndex(p.toIndex);
    right005x = extrapolateEdge(ir.baselineSlope, ir.baselineIntercept, ir.peakHeightBaseline,
                                QPointF(p.data.at(fIdx).x(), p.data.at(fIdx).y()),
                                QPointF(p.toX, p.data.at(tIdx).y()));
    useExtrapolateA0 = true;
  } else
    right005x = p.data.at(right_w005 + p.fromIndex).x();


  double width005Left = r.peakX - left005x;
  double width005Right = right005x - r.peakX;

  r.HVL_width005 = width005Left + width005Right;
  r.HVL_width005Left = width005Left;
  r.HVL_width005Right = width005Right;

  r.seriesAOne.push_back(QPointF(left005x, r.baselineSlope * left005x + r.baselineIntercept));
  r.seriesATwo.push_back(QPointF(left005x, r.peakHeightBaseline));
  r.seriesBOne.push_back(QPointF(left005x, r.peakHeightBaseline * left005x + r.baselineIntercept));
  r.seriesBTwo.push_back(QPointF(left005x, r.peakHeightBaseline));

  /* HVL Estimation */
  r.HVL_tP = r.peakX;
  r.HVL_tUSP = (width005Right + width005Left) / (2 * width005Left);

  r.HVL_migT = r.HVL_tP;
  r.HVL_width = r.widthHalfFull;
  r.HVL_widthLeft = r.widthHalfLeft;
  r.HVL_widthRight = r.widthHalfRight;

  echmet::HVLCore::Coefficients HVL_coefficients = echmet::HVLCore::Coefficients::Calculate(r.peakArea, r.HVL_tP, r.widthHalfFull, r.HVL_tUSP);

  if (useExtrapolateA0) {
    try {
      r.HVL_a0 = extrapolateA0(left005x, right005x, r.baselineSlope, r.baselineIntercept, p.data);
    } catch (std::runtime_error &) {
      r.HVL_a0 = r.peakArea;
    }
  } else
    r.HVL_a0 = r.peakArea;
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

  r.baselineCorrectedPeak = correctForBaseline(p.fromIndex, p.toIndex, r.baselineSlope, r.baselineIntercept, p.data);
  r.peakArea = calculateArea(r.baselineCorrectedPeak, false);
  r.correctedPeakArea = calculateArea(r.baselineCorrectedPeak, true);
  calculateVariances(r);
  r.nAsym = std::pow(r.meanX, 2) / r.varianceMean; /* Requested by Hervé Cottet for separation efficiency evaluations
                                                      with asymetrical peaks */

  r.validate();

  return r;
}
