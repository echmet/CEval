#include "hvlcalculator.h"
#include "helpers.h"
#include "math/hvl.hpp"
#include <QMessageBox>

#include <QDebug>

struct EvalPair { double value; int index; };
#include "math/regressor/regress.h"
#include "math/regressor/intervalestimation.h"

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

HVLCalculator::HVLCalculator()
{
  t_matrix = new echmet::matrix::Core<>();
  a_matrix = new echmet::matrix::Core<>();
  parameters_matrix = new echmet::matrix::Core<>();
  sigma_matrix = new echmet::matrix::Core<>();

  m_hvlLib = new echmet::math::HVL_dll();
}

HVLCalculator::~HVLCalculator()
{
  delete t_matrix;
  delete a_matrix;
  delete parameters_matrix;
  delete sigma_matrix;
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

HVLCalculator::HVLParameters HVLCalculator::fit(const QVector<QPointF> &data, const unsigned long fromIdx, const unsigned long toIdx,
                                                const double a0, const double a1, const double a2, const double a3,
                                                const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
                                                const double epsilon, const int iterations, const int digits)
{
  Q_ASSERT(s_me != nullptr);

  unsigned long size = toIdx - fromIdx; /* <- toIdx value seems to be wrong, +1 should be needed here */

  s_me->t_matrix->data.New(size, 1);

  for (unsigned long j = 0; j < size; ++j) {

          (*s_me->t_matrix)[j][0] = data.at(fromIdx + j).x();
          //++index;

  }

  //index = P.TAi;

  s_me->a_matrix->data.New(size, 1);

  for (unsigned long j = 0; j < size; ++j){

           (*s_me->a_matrix)[j][0] = data.at(fromIdx + j).y();
           //++index;

  }

  s_me->sigma_matrix->data.New(size, 1);

  for (unsigned long j = 0; j < size; ++j) (*s_me->sigma_matrix)[j][0] = 1;

  s_me->parameters_matrix->data.New(4, 1);

  (*(s_me->parameters_matrix))[0][0] = a0;
  (*(s_me->parameters_matrix))[1][0] = a1;
  (*(s_me->parameters_matrix))[2][0] = a2;
  (*(s_me->parameters_matrix))[3][0] = a3;

  echmet::regressCore::HVLFunction<> HVLF
  (*(s_me->t_matrix), *(s_me->a_matrix), *(s_me->sigma_matrix), *(s_me->parameters_matrix), epsilon, iterations,
   a0fixed, a1fixed, a2fixed, a3fixed, true, true);

  HVLF.Regress();

  return HVLParameters(HVLF.GetA0(), HVLF.GetA1(), HVLF.GetA2(), HVLF.GetA3(), HVLF.GetChi());
}

QVector<QPointF> HVLCalculator::plot(const double a0, const double a1, const double a2, const double a3, const double fromX, const double toX, const double step,
                                     const int digits)
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
#ifdef Q_OS_WIN
  const hvlstr_t path = L"./hvl_mt.dll";
#elif defined Q_OS_LINUX
  const hvlstr_t path = (const hvlstr_t)"./hvl_mt.so";
#else
  #error "Unsupported platform"
#endif
  hvlstr_t errorMessage;

  s_me = new HVLCalculator();

  if (!s_me->m_hvlLib->Open(path, &errorMessage)){
    QMessageBox::warning(nullptr, QObject::tr("HVL fitting error"), QString(QObject::tr("Unable to load hvl_mt library. HVL plotting will not be available\n"
                                                                                        "Error returned: %1")).arg(Helpers::hvlstrToQString(errorMessage)));
    s_me->m_hvlLib->FreeErrorMessage(errorMessage);
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
