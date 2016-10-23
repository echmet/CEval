#include "hvlcalculator.h"
#include "globals.h"
#include "helpers.h"
#include "gui/hvlfitinprogressdialog.h"
#include "hvllibwrapper.h"
#include "math/regressor/hvlPeak.h"
#include <QMessageBox>
#include <QTime>
#include <QApplication>

#include <thread>

//#include <vector>
#include <armadillo>

using namespace arma;
using namespace std;

HVLCalculator *HVLCalculator::s_me;

HVLCalculator::HVLParameters::HVLParameters() :
  a0(0.0), a1(0.0), a2(0.0), a3(0.0),
  m_valid(false)
{
}

HVLCalculator::HVLParameters::HVLParameters(const double a0, const double a1, const double a2, const double a3) :
  a0(a0), a1(a1), a2(a2), a3(a3),
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

HVLCalculator::HVLCalculator(QObject *parent, const int precision) :
  QObject(parent),
  m_regressor(nullptr),
  m_wrapper(new HVLLibWrapper(precision))
{
}

HVLCalculator::~HVLCalculator()
{
  delete m_regressor;
  delete m_wrapper;
}

void HVLCalculator::applyBaseline(QVector<QPointF> &data, const double k, const double q)
{
  for (int idx = 0; idx < data.length(); idx++) {
    const QPointF &p = data.at(idx);
    double y = k * p.x() + q;

    data[idx] = QPointF(p.x(), p.y() + y);
  }
}

void HVLCalculator::doFit(HVLParameters *out, const HVLInParameters *in)
{
  int size = in->toIdx - in->fromIdx;
  if (size < 1)
    return;

  vector<double> x(size, 1);
  Mat<double>    y(size, 1);

#pragma omp parallel for
  for (int j = 0; j < size; ++j) {

          x[j]   = in->data->at(in->fromIdx + j).x();
          y(j,0) = in->data->at(in->fromIdx + j).y();

  }

  in->regressor->Initialize(
      x, y, in->epsilon, in->iterations, true,
      echmet::HVLCore::Coefficients(in->a0, in->a1, in->a2, in->a3),
      in->bsl, in->bslSlope
  );
  if (in->a0fixed) in->regressor->FixParameter(echmet::regressCore::HVLPeakParams::a0, in->a0);
  if (in->a1fixed) in->regressor->FixParameter(echmet::regressCore::HVLPeakParams::a1, in->a1);
  if (in->a2fixed) in->regressor->FixParameter(echmet::regressCore::HVLPeakParams::a2, in->a2);
  if (in->a3fixed) in->regressor->FixParameter(echmet::regressCore::HVLPeakParams::a3, in->a3);

  double s0 = in->regressor->GetS();

  bool ok = in->regressor->Regress();

  if (!ok) {
    emit hvlFitDone();
    return;
  }

  out->a0 = in->regressor->GetParameter(echmet::regressCore::HVLPeakParams::a0);
  out->a1 = in->regressor->GetParameter(echmet::regressCore::HVLPeakParams::a1);
  out->a2 = in->regressor->GetParameter(echmet::regressCore::HVLPeakParams::a2);
  out->a3 = in->regressor->GetParameter(echmet::regressCore::HVLPeakParams::a3);
  out->s = in->regressor->GetS();
  out->s0 = s0;
  out->iterations = in->regressor->GetIterationCounter();
  out->validate();

  emit hvlFitDone();
}

HVLCalculator::HVLParameters HVLCalculator::fit(const QVector<QPointF> &data, const int fromIdx, const int toIdx,
    double a0, double a1, double a2, double a3,
    const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
    const double bsl, const double bslSlope,
    const double epsilon, const int iterations, int digits, const bool showStats = false)
{
  Q_ASSERT(s_me != nullptr);

  HVLFitInProgressDialog inProgressDlg;
  HVLParameters p;
  HVLInParameters in;

  if (!(epsilon > 0.0)) {
    QMessageBox::warning(nullptr, tr("Invalid parameter"), tr("Value of \"epsilon\" must be positive"));
    return p;
  }

  if (s_me->m_regressor != nullptr) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Previous instance of HVL regressor was not cleaned up correctly, attempting to continue\n\n"
                                                                  "Please report this as a bug to %1 developers").arg(Globals::SOFTWARE_NAME)));

    delete s_me->m_regressor;
  }

  try {
    s_me->m_wrapper->setPrecision(digits);
  } catch (HVLLibWrapper::HVLLibException &) {
    return p;
  }

  try {
    s_me->m_regressor = new echmet::regressCore::HVLPeak<double, double>(s_me->m_wrapper);
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Insufficient memory to initialize HVL regressor"));

    return p;
  }

  in.regressor = s_me->m_regressor;
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
  connect(&inProgressDlg, &HVLFitInProgressDialog::abortFit, s_me, &HVLCalculator::onAbortFit);

  QTime stopwatch;
  stopwatch.start();

  std::thread fitThr(&HVLCalculator::doFit, s_me, &p, &in);

  inProgressDlg.exec();
  fitThr.join();

  if (!p.isValid()) {
    QMessageBox::warning(nullptr, tr("HVL fit failed"), tr("Regressor failed to converge within %1 iterations. Try to increase the number of iterations and run the fit again.\n\n"
                                                           "Note that it might be impossible to fit your data with HVL function. In such a case increasing the number "
                                                           "of iterations will not have any effect.").arg(in.iterations));

    delete s_me->m_regressor;
    s_me->m_regressor = nullptr;

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

  delete s_me->m_regressor;
  s_me->m_regressor = nullptr;

  return p;
}

void HVLCalculator::onAbortFit()
{
  if (s_me->m_regressor == nullptr)
    return;

  s_me->m_regressor->Abort();
}

QVector<QPointF> HVLCalculator::plot(
    const double a0,    const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step,  const int digits
)
{
  Q_ASSERT(s_me != nullptr);
  QVector<QPointF> vec;

  try {
    s_me->m_wrapper->setPrecision(digits);
  } catch (HVLLibWrapper::HVLLibException &) {
    return vec;
  }

  HVLLibWrapper::Result result = s_me->m_wrapper->calculateRange(HVLLibWrapper::Parameter::T, fromX, toX, step, a0, a1, a2, a3);

  for (int idx = 0; idx < result.count(); idx++) {
    const HVLLibWrapper::XY &xy = result[idx];

    vec.push_back(QPointF(xy.x, xy.y));
  }

  return vec;
}

bool HVLCalculator::initialize()
{
  try {
    s_me = new HVLCalculator(nullptr, 30); /* Use some default precision, will be overriden by later */
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"), QObject::tr("Unable to initialize HVL calculator"));
    return false;
  }

  return true;
}
