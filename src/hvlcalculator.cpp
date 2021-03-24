#include "hvlcalculator.h"
#include "globals.h"
#include "helpers.h"
#include "gui/hvlfitinprogressdialog.h"
#include "gui/hvlestimateinprogressdialog.h"
#include "math/hvlestimate.h"
#include "math/regressor/hvlPeak.h"
#include <QElapsedTimer>
#include <QMessageBox>
#include <QApplication>
#include <Eigen/Dense>
#include <functional>

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
  failure = Failure::OK;
  m_valid = true;
}

HVLCalculator::HVLInParameters::HVLInParameters()
{
}

HVLCalculatorWorker::HVLCalculatorWorker(const HVLCalculator::HVLInParameters &params) :
  m_regressor(new echmet::regressCore::HVLPeak<double, double>()),
  m_params(params)
{
}

HVLCalculatorWorker::~HVLCalculatorWorker()
{
    delete m_regressor;
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

  Eigen::VectorXd x(size);
  Eigen::MatrixXd y(size, 1);

  for (int j = 0; j < size; ++j) {
    x[j]   = m_params.data->at(m_params.fromIdx + j).x();
    y(j,0) = m_params.data->at(m_params.fromIdx + j).y();
  }

  if (!m_regressor->Initialize(x, y, m_params.epsilon, m_params.iterations, true,
                               echmet::HVLCore::Coefficients(m_params.a0, m_params.a1, m_params.a2, m_params.a3),
                               m_params.bsl, m_params.bslSlope)) {
    m_outParams.failure = HVLCalculator::HVLParameters::Failure::CANNOT_INIT;
    return;
  }

  if (m_params.a0fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a0, m_params.a0);
  if (m_params.a1fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a1, m_params.a1);
  if (m_params.a2fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a2, m_params.a2);
  if (m_params.a3fixed)
    m_regressor->FixParameter(echmet::regressCore::HVLPeakParams::a3, m_params.a3);

  double s0 = m_regressor->GetS();

  m_fitStartTime = QTime::currentTime();

  bool ok = m_regressor->Regress();

  if (m_regressor->IsAborted()) {
    m_outParams.failure = HVLCalculator::HVLParameters::Failure::ABORTED;
    return;
  }
  if (!ok) {
    m_outParams.failure = HVLCalculator::HVLParameters::Failure::NO_CONVERGENCE;
    return;
  }

  m_outParams.a0 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a0);
  m_outParams.a1 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a1);
  m_outParams.a2 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a2);
  m_outParams.a3 = m_regressor->GetParameter(echmet::regressCore::HVLPeakParams::a3);
  m_outParams.s = m_regressor->GetS();
  m_outParams.finalPrecision = 0;
  m_outParams.s0 = s0;
  m_outParams.iterations = m_regressor->GetIterationCounter();
  m_outParams.validate();
}

HVLCalculator::~HVLCalculator()
{
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
    const double epsilon, const int iterations,
    const bool showStats = false)
{
  Q_ASSERT(s_me != nullptr);

  HVLFitInProgressDialog inProgressDlg(iterations);
  HVLParameters p;
  HVLInParameters in;

  if (!(epsilon > 0.0)) {
    QMessageBox::warning(nullptr, QObject::tr("Invalid parameter"), QObject::tr("Value of \"epsilon\" must be positive"));
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

  HVLCalculatorWorker worker(in);
  worker.process();
  p = worker.results();

  switch (p.failure) {
  case HVLParameters::Failure::ABORTED:
    return p;
  case HVLParameters::Failure::CANNOT_INIT:
    QMessageBox::warning(nullptr, QObject::tr("HVL fit failed"), QObject::tr("Regressor could not have been initialized"));
    return p;
  case HVLParameters::Failure::NO_CONVERGENCE:
    QMessageBox::warning(nullptr, QObject::tr("HVL fit failed"), QObject::tr("Regressor failed to converge within %1 iterations. Try to increase the number of iterations and run the fit again.\n\n"
                                                                             "Note that it might be impossible to fit your data with HVL function. In such a case increasing the number "
                                                                             "of iterations will not have any effect.").arg(in.iterations));

    return p;
  case HVLParameters::Failure::OK:
    break;
  }

  if (showStats) {
    QMessageBox::information(
      nullptr,
      QObject::tr("HVL Fit"),
      QString(QObject::tr(
          "Iterations: %1 :\nold value -> new value :\n"
          "Sigma : %2 -> %3\n"
          "a1  : %4 -> %5\n"
       ))
      .arg(p.iterations)
      .arg(p.s0, 10)
      .arg(p.s, 10)
      .arg(a1, 10)
      .arg(p.a1, 10)
    );
  }

  return p;
}

QVector<QPointF> HVLCalculator::plot(
    const double a0,    const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step
)
{
  Q_ASSERT(s_me != nullptr);
  QVector<QPointF> vec;

  using HVLCalc = VHHVLCalculator<double>;

  for (double x = fromX; x <= toX; x += step) {
      double y = HVLCalc::y(x, a0, a1, a2, a3);
      vec.push_back(QPointF(x, y));
  }

  return vec;
}

bool HVLCalculator::initialize()
{
  try {
    s_me = new HVLCalculator(); /* Use some default precision, will be overriden by later */
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"), QObject::tr("Unable to initialize HVL calculator"));
    return false;
  }

  return true;
}
