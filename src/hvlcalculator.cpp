#include "hvlcalculator.h"
#include "helpers.h"
#include "gui/hvlfitinprogressdialog.h"
#include "math/hvl.hpp"
#include "math/regressor/hvlPeak.h"
#include <QMessageBox>
#include <QTime>
#include <QApplication>

#include <thread>

#include <QDebug>

#include <vector>
#include <armadillo>

using namespace arma;
using namespace std;

HVLCalculator *HVLCalculator::s_me;

HVLCalculator::HVLParameters::HVLParameters() :
  a0(0.0), a1(0.0), a2(0.0), a3(0.0), chiSquared(0.0),
  m_valid(false)
{
}

HVLCalculator::HVLParameters::HVLParameters(const double a0, const double a1, const double a2, const double a3, const double chiSquared) :
  a0(a0), a1(a1), a2(a2), a3(a3), chiSquared(chiSquared),
  m_valid(true)
{
}

bool HVLCalculator::HVLParameters::isValid() const
{
  return m_valid;
}

void HVLCalculator::HVLParameters::validate()
{
  m_valid = true;
}

HVLCalculator::HVLInParameters::HVLInParameters()
{
}

HVLCalculator::HVLCalculator(QObject *parent) :
  QObject(parent)
{
  m_hvlLib = new echmet::math::HVL_dll();
}

HVLCalculator::~HVLCalculator()
{
  delete m_hvlLib;
}

void HVLCalculator::applyBaseline(QVector<QPointF> &data, const double k, const double q)
{
  for (int idx = 0; idx < data.length(); idx++) {
    const QPointF &p = data.at(idx);
    double y = k * p.x() + q;

    data[idx] = QPointF(p.x(), p.y() + y);
  }
}

bool HVLCalculator::available()
{
  return s_me != nullptr;
}

void HVLCalculator::doFit(HVLParameters *out, const HVLInParameters *in)
{
  unsigned long size = in->toIdx - in->fromIdx;

  vector<double> x(size, 1);
  Mat<double>    y(size, 1);

#pragma omp parallel for
  for (unsigned long j = 0; j < size; ++j) {

          x[j]   = in->data->at(in->fromIdx + j).x();
          y(j,0) = in->data->at(in->fromIdx + j).y();

  }

  echmet::regressCore::HVLPeak<double, double> regressor(s_me->m_hvlLib);
  regressor.Initialize(
      x, y, in->epsilon, in->iterations, true,
      echmet::HVLCore::Coefficients(in->a0, in->a1, in->a2, in->a3),
      in->bsl, in->bslSlope
  );
  if (in->a0fixed) regressor.FixParameter(echmet::regressCore::HVLPeakParams::a0, in->a0);
  if (in->a1fixed) regressor.FixParameter(echmet::regressCore::HVLPeakParams::a1, in->a1);
  if (in->a2fixed) regressor.FixParameter(echmet::regressCore::HVLPeakParams::a2, in->a2);
  if (in->a3fixed) regressor.FixParameter(echmet::regressCore::HVLPeakParams::a3, in->a3);

  double s0 = regressor.GetS();

  bool ok = regressor.Regress();

  if (!ok) {
    emit hvlFitDone();
    return;
  }

  out->a0 = regressor.GetParameter(echmet::regressCore::HVLPeakParams::a0);
  out->a1 = regressor.GetParameter(echmet::regressCore::HVLPeakParams::a1);
  out->a2 = regressor.GetParameter(echmet::regressCore::HVLPeakParams::a2);
  out->a3 = regressor.GetParameter(echmet::regressCore::HVLPeakParams::a3);
  out->s = regressor.GetS();
  out->s0 = s0;
  out->chiSquared = 0.0;
  out->iterations = regressor.GetIterationCounter();
  out->validate();

  emit hvlFitDone();
}

HVLCalculator::HVLParameters HVLCalculator::fit(const QVector<QPointF> &data, const unsigned long fromIdx, const unsigned long toIdx,
    double a0, double a1, double a2, double a3,
    const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
    const double bsl, const double bslSlope,
    const double epsilon, const int iterations, int digits = 0, const bool showStats = false)
{
  Q_ASSERT(s_me != nullptr);

  HVLFitInProgressDialog inProgressDlg;
  HVLParameters p;
  HVLInParameters in;

  if (digits > 0)
    s_me->m_hvlLib->HVLSetPrec(digits);

  if (!(epsilon > 0.0)) {
    QMessageBox::warning(nullptr, tr("Invalid parameter"), tr("Value of \"epsilon\" must be positive"));
    return p;
  }

  in.a0 = a0;
  in.a0fixed = a0fixed;
  in.a1 = a1;
  in.a1fixed = a1fixed;
  in.a2 = a2;
  in.a2fixed = a2fixed;
  in.a3 = a3;
  in.a3fixed = a3fixed;
  in.bsl = bsl;
  in.bslSlope = bslSlope;
  in.data = &data;
  in.epsilon = epsilon;
  in.fromIdx = fromIdx;
  in.iterations = iterations;
  in.toIdx = toIdx;

  connect(s_me, &HVLCalculator::hvlFitDone, &inProgressDlg, &HVLFitInProgressDialog::onHvlFitDone);

  QTime stopwatch;
  stopwatch.start();

  std::thread fitThr(&HVLCalculator::doFit, s_me, &p, &in);

  inProgressDlg.exec();
  fitThr.join();

  if (!p.isValid()) {
    QMessageBox::warning(nullptr, tr("HVL fit failed"), tr("Regressor failed to converge within %1 iterations. Try to increase the number of iterations and run the fit again.\n\n"
                                                           "Note that it might be impossible to fit your data with HVL function. In such a case increasing the number "
                                                           "of iterations will not have any effect.").arg(in.iterations));
    return p;
  }

  if (showStats) {
    QMessageBox::information(
      nullptr,
      QObject::tr("HVL Fit"),
      QString(QObject::tr(
          "Iterations: %1 :\nold value -> new value :\n"
          "Sigma : %2 -> %3\n"
          "a1  : %4 -> %5\n"
          "Time elapsed (sec): %6"
       ))
      .arg(p.iterations)
      .arg(p.s0, 10)
      .arg(p.s, 10)
      .arg(a1, 10)
      .arg(p.a1, 10)
      .arg(static_cast<double>(stopwatch.elapsed()) / 1000.0)
    );
  }

  return p;
}

QVector<QPointF> HVLCalculator::plot(
    const double a0,    const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step,  const int digits
)
{
  Q_ASSERT(s_me != nullptr);
  QVector<QPointF> vec;

  if (digits > 0)
    s_me->m_hvlLib->HVLSetPrec(digits);

  echmet::math::HVL_dll::HVLData hvlData = s_me->m_hvlLib->HVL_range(fromX, toX, step, a0, a1, a2, a3);

  for (size_t idx = 0; idx < hvlData.count(); idx++)
    vec.push_back(QPointF(hvlData.x(idx), hvlData.y(idx)));

  return vec;
}

bool HVLCalculator::initialize()
{
  hvlstr_t errorMessage = nullptr;

  s_me = new HVLCalculator();

  if (!s_me->m_hvlLib->Open(ECHMET_MATH_HVL_DLL, &errorMessage)) {
    QString qstrErrorMessage;

    if (errorMessage != nullptr) {
      qstrErrorMessage = Helpers::hvlstrToQString(errorMessage);
      s_me->m_hvlLib->FreeErrorMessage(errorMessage);
    } else
      qstrErrorMessage = tr("Could not retrieve error message");
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"), QString(QObject::tr("Unable to load hvl_mt library. HVL plotting will not be available\n"
                                                                                        "Error returned: %1")).arg(qstrErrorMessage));
    delete s_me;
    s_me = nullptr;
    return false;
  }

  if (!s_me->m_hvlLib->Load()) {
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"),
                         QObject::tr("Could not resolve symbols in hvl_mt library. HVL plotting will not be available"));
    delete s_me;
    s_me = nullptr;
    return false;
  }

  if (!s_me->m_hvlLib->HVLSetPrec(50))
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"),
                         QObject::tr("Could not set MPFR precision in hvl_mt library, calculation results might be incorrect"));

  return true;
}
