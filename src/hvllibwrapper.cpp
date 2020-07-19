#include "hvllibwrapper.h"
#include "hvl.h"

static
void throwOnError(const HVL_RetCode tRet)
{
  switch (tRet) {
  case HVL_OK:
    return;
  case HVL_E_NO_MEMORY:
    throw HVLLibWrapper::HVLLibException("Insufficient memory", false);
  case HVL_E_INVALID_ARG:
    throw HVLLibWrapper::HVLLibException("Invalid argument", false);
  case HVL_E_INTERNAL:
    throw HVLLibWrapper::HVLLibException("Internal HVL_MT error", false);
  case HVL_E_MPFR_ASSERT:
    throw HVLLibWrapper::HVLLibException("MPFR assertion failed", true);
  default:
    throw HVLLibWrapper::HVLLibException("Unspecified error", false);
  }
}

HVLLibWrapper::HVLLibException::HVLLibException(const char *what, const bool isFatal) :
  std::runtime_error(what),
  isFatal(isFatal)
{
}

HVLLibWrapper::XY::XY() :
  x(0.0),
  y(0.0)
{
}

HVLLibWrapper::XY::XY(const double x, const double y) :
  x(x),
  y(y)
{
}

HVLLibWrapper::XY::XY(const XY &other) :
  x(other.x),
  y(other.y)
{
}

HVLLibWrapper::XY & HVLLibWrapper::XY::operator=(const XY &other)
{
  const_cast<double&>(x) = other.x;
  const_cast<double&>(y) = other.y;

  return *this;
}

HVLLibWrapper::XYPack::PUType HVLLibWrapper::XYPack::parameterToIndex(const Parameter p) const
{
  return static_cast<PUType>(p);
}

void HVLLibWrapper::XYPack::setData(const Parameter p, const XY &data)
{
  const PUType idx = parameterToIndex(p);

  m_data[idx] = data;
}

const HVLLibWrapper::XY & HVLLibWrapper::XYPack::operator[](const Parameter p) const
{
  return m_data[parameterToIndex(p)];
}

const HVLLibWrapper::XY & HVLLibWrapper::XYPack::operator[](const size_t paramIdx) const
{
  return m_data[paramIdx];
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
  m_hctx(nullptr),
  m_precision(-1)
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
  HVL_RetCode tRet;
  double result = 0.0;

  switch (p) {
  case Parameter::T:
    tRet = HVL_t(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DX:
    tRet = HVL_dx(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA0:
    tRet = HVL_da0(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA1:
    tRet = HVL_da1(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA2:
    tRet = HVL_da2(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  case Parameter::DA3:
    tRet = HVL_da3(&result, m_hctx, x, a0, a1, a2, a3);
    break;
  }

  throwOnError(tRet);

  return XY(x, result);
}

const HVLLibWrapper::XYPack HVLLibWrapper::calculateMultiple(const ParameterFlags params,
                                                              const double x,
                                                              const double a0, const double a1, const double a2, const double a3) const
{
  HVL_Prepared *prep;
  XYPack pack;
  HVL_RetCode tRet;

  tRet = HVL_prepare(&prep, m_hctx, x, a0, a1, a2, a3);
  throwOnError(tRet);

  try {
    if (params & ParameterFlags::T) {
      double t;
      tRet = HVL_t_prepared(&t, prep);
      throwOnError(tRet);

      pack.setData(Parameter::T, XY(x, t));
    }
    if (params & ParameterFlags::DX) {
      double dx;
      tRet = HVL_dx_prepared(&dx, prep);
      throwOnError(tRet);

      pack.setData(Parameter::DX, XY(x, dx));
    }
    if (params & ParameterFlags::DA0) {
      double da0;
      tRet = HVL_da0_prepared(&da0, prep);
      throwOnError(tRet);

      pack.setData(Parameter::DA0, XY(x, da0));
    }
    if (params & ParameterFlags::DA1) {
      double da1;
      tRet = HVL_da1_prepared(&da1, prep);
      throwOnError(tRet);

      pack.setData(Parameter::DA1, XY(x, da1));
    }
    if (params & ParameterFlags::DA2) {
      double da2;
      tRet = HVL_da2_prepared(&da2, prep);
      throwOnError(tRet);

      pack.setData(Parameter::DA2, XY(x, da2));
    }
    if (params & ParameterFlags::DA3) {
      double da3;
      tRet = HVL_da3_prepared(&da3, prep);
      throwOnError(tRet);

      pack.setData(Parameter::DA3, XY(x, da3));
    }
  } catch (const HVLLibException &ex) {
    HVL_free_prepared(prep);

    throw ex;
  }

  HVL_free_prepared(prep);

  return pack;
}

HVLLibWrapper::Result HVLLibWrapper::calculateRange(const Parameter p,
                                                    const double from, const double to, const double step,
                                                    const double a0, const double a1, const double a2, const double a3) const
{
  HVL_Range *r = nullptr;
  HVL_RetCode tRet;

  switch (p) {
  case Parameter::T:
    tRet = HVL_t_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DX:
    tRet = HVL_dx_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA0:
    tRet = HVL_da0_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA1:
    tRet = HVL_da1_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA2:
    tRet = HVL_da2_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  case Parameter::DA3:
    tRet = HVL_da3_range(&r, m_hctx, from, to, step, a0, a1, a2, a3);
    break;
  }

  throwOnError(tRet);

  QList<XY> values;
  for (size_t idx = 0; idx < r->count; idx++) {
    const HVL_Pair *pr = &r->p[idx];

    values.append(XY(pr->x, pr->y));
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
  if (precision < 1)
    return;

  auto tRet = HVL_make_context(&m_hctx, precision);
    throwOnError(tRet);

  m_precision = precision;
}
