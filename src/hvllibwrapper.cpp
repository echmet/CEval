#include "hvllibwrapper.h"
#include "hvl.h"

HVLLibWrapper::HVLLibException::HVLLibException() :
  std::exception()
{
}

const char * HVLLibWrapper::HVLLibException::what() const noexcept
{
  return "Unable to perform HVL calculation";
}

HVLLibWrapper::XY::XY(const double x, const double y) :
  x(x),
  y(y)
{
}

HVLLibWrapper::XY & HVLLibWrapper::XY::operator=(const XY &other)
{
  const_cast<double&>(x) = other.x;
  const_cast<double&>(y) = other.y;

  return *this;
}

HVLLibWrapper::AllParameters::AllParameters(const XY &da0, const XY &da1, const XY &da2, const XY &da3) :
  da0(da0),
  da1(da1),
  da2(da2),
  da3(da3)
{
}

HVLLibWrapper::Result::Result() :
  m_values(QList<XY>())
{
}

HVLLibWrapper::Result::Result(const QList<XY> &values) :
  m_values(values)
{
}

int HVLLibWrapper::Result::count() const
{
  return m_values.size();
}

const HVLLibWrapper::XY & HVLLibWrapper::Result::operator[](const int idx) const
{
  if (idx >= m_values.size() || idx < 0)
    throw std::out_of_range("Value of idx is out of range");

  return m_values.at(idx);
}

HVLLibWrapper::HVLLibWrapper(const int precision) :
  m_hctx(nullptr)
{
  setPrecision(precision);
}

HVLLibWrapper::~HVLLibWrapper()
{
  HVL_free_context(m_hctx);
}

HVLLibWrapper::XY HVLLibWrapper::calculate(const Parameter p,
                                           const double x,
                                           const double a0, const double a1, const double a2, const double a3) const
{
  double result = 0.0;

  switch (p) {
  case Parameter::T:
    result = HVL_t(m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DX:
    result = HVL_dx(m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA0:
    result = HVL_da0(m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA1:
    result = HVL_da1(m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA2:
    result = HVL_da2(m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA3:
    result = HVL_da3(m_hctx, x, a0, a1, a2, a3);
    break;
  }

  return XY(x, result);
}

HVLLibWrapper::AllParameters HVLLibWrapper::calculateAll(const double x,
                                                         const double a0, const double a1, const double a2, const double a3) const
{
  HVL_Prepared *prep;

  if (HVL_prepare(&prep, m_hctx, x, a0, a1, a2, a3) != HVL_OK)
    throw HVLLibException();

  const double da0 = HVL_da0_prepared(prep);
  const double da1 = HVL_da1_prepared(prep);
  const double da2 = HVL_da2_prepared(prep);
  const double da3 = HVL_da3_prepared(prep);

  HVL_free_prepared(prep);

  return AllParameters(XY(x, da0),
                       XY(x, da1),
                       XY(x, da2),
                       XY(x, da3));
}

HVLLibWrapper::Result HVLLibWrapper::calculateRange(const Parameter p,
                                                    const double from, const double to, const double step,
                                                    const double a0, const double a1, const double a2, const double a3) const
{
  HVL_Range *r = nullptr;

  switch (p) {
  case Parameter::T:
    r = HVL_t_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DX:
    r = HVL_dx_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA0:
    r = HVL_da0_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA1:
    r = HVL_da1_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA2:
    r = HVL_da2_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA3:
    r = HVL_da3_range(m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  }

  if (r == nullptr)
    return Result();

  QList<XY> values;
  for (size_t idx = 0; idx < r->count; idx++) {
    const HVL_Pair *p = &r->p[idx];

    values.append(XY(p->x, p->y));
  }

  HVL_free_range(r);

  return Result(values);
}

int HVLLibWrapper::precision() const
{
  return m_precision;
}

void HVLLibWrapper::setPrecision(const int precision)
{
  if (m_precision == precision)
    return;

  if (HVL_make_context(&m_hctx, precision) != HVL_OK)
    throw HVLLibException();

  m_precision = precision;
}
