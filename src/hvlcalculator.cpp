#include "hvlcalculator.h"
#include "globals.h"
#include "helpers.h"
#include "gui/hvlfitinprogressdialog.h"
#include "hvllibwrapper.h"
#include "math/regressor/hvlPeak.h"
#include <QMessageBox>
#include <QTime>
#include <QApplication>
#include <QThread>
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
  m_wrapper(new HVLLibWrapper(precision))
{
}

HVLCalculatorWorker::HVLCalculatorWorker(const HVLCalculator::HVLInParameters &params, HVLLibWrapper *wrapper) :
  m_regressor(new echmet::regressCore::HVLPeak<double, double>(wrapper)),
  m_params(params)
{
}

HVLCalculatorWorker::~HVLCalculatorWorker()
{
  delete m_regressor;
}

void HVLCalculatorWorker::abort()
{
  m_regressor->Abort();
}

const HVLCalculator::HVLParameters & HVLCalculatorWorker::results() const
{
  return m_outParams;
}

void HVLCalculatorWorker::process()
{
  int size = m_params.toIdx - m_params.fromIdx;
  if (size < 1)
    return;

  vector<double> x(size, 1);
  Mat<double>    y(size, 1);

  #pragma omp parallel for
  for (int j = 0; j < size; ++j) {
    x[j]   = m_params.data->at(m_params.fromIdx + j).x();
    y(j,0) = m_params.data->at(m_params.fromIdx + j).y();
  }

  m_regressor->Initialize(x, y, m_params.epsilon, m_params.iterations, true,
                          echmet::HVLCore::Coefficients(m_params.a0, m_params.a1, m_params.a2, m_params.a3),
                          m_params.bsl, m_params.bslSlope);

  if (m_params.a0fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a0, m_params.a0);
  if (m_params.a1fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a1, m_params.a1);
  if (m_params.a2fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a2, m_params.a2);
  if (m_params.a3fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a3, m_params.a3);

  double s0 = m_regressor->GetS();

  bool ok = m_regressor->Regress();

  m_outParams.aborted = m_regressor->IsAborted();
  if (m_outParams.aborted || !ok) {
    emit finished();
    return;
  }

  m_outParams.a0 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a0);
  m_outParams.a1 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a1);
  m_outParams.a2 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a2);
  m_outParams.a3 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a3);
  m_outParams.s = m_regressor->GetS();
  m_outParams.s0 = s0;
  m_outParams.iterations = m_regressor->GetIterationCounter();
  m_outParams.validate();

  emit finished();
}

HVLCalculator::~HVLCalculator()
{
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

  try {
    s_me->m_wrapper->setPrecision(digits);
  } catch (HVLLibWrapper::HVLLibException &) {
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

  HVLCalculatorWorker worker(in, s_me->m_wrapper);
  QThread thread;
  worker.moveToThread(&thread);
  connect(&thread, &QThread::started, &worker, &HVLCalculatorWorker::process);
  connect(&worker, &HVLCalculatorWorker::finished, &thread, &QThread::quit);
  connect(&worker, &HVLCalculatorWorker::finished, &inProgressDlg, &HVLFitInProgressDialog::onHvlFitDone);
  connect(&inProgressDlg, &HVLFitInProgressDialog::abortFit, &worker, &HVLCalculatorWorker::abort, Qt::DirectConnection);

  QTime stopwatch;
  stopwatch.start();

  thread.start();
  inProgressDlg.exec();
  thread.wait();

  p = worker.results();

  if (p.aborted)
    return p;

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
