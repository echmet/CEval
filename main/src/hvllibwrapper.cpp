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

const HVLLibWrapper::XYPack HVLLibWrapper::calculateMultiple(const ParameterFlags params,
                                                              const double x,
                                                              const double a0, const double a1, const double a2, const double a3) const
{
  HVL_Prepared *prep;
  XYPack pack;

  if (HVL_prepare(&prep, m_hctx, x, a0, a1, a2, a3) != HVL_OK)
    throw HVLLibException();

  if (params & ParameterFlags::T) {
    const double t = HVL_t_prepared(prep);
    pack.setData(Parameter::T, XY(x, t));
  }
  if (params & ParameterFlags::DX) {
    const double dx = HVL_dx_prepared(prep);
    pack.setData(Parameter::DX, XY(x, dx));
  }
  if (params & ParameterFlags::DA0) {
    const double da0 = HVL_da0_prepared(prep);
    pack.setData(Parameter::DA0, XY(x, da0));
  }
  if (params & ParameterFlags::DA1) {
    const double da1 = HVL_da1_prepared(prep);
    pack.setData(Parameter::DA1, XY(x, da1));
  }
  if (params & ParameterFlags::DA2) {
    const double da2 = HVL_da2_prepared(prep);
    pack.setData(Parameter::DA2, XY(x, da2));
  }
  if (params & ParameterFlags::DA3) {
    const double da3 = HVL_da3_prepared(prep);
    pack.setData(Parameter::DA3, XY(x, da3));
  }

  HVL_free_prepared(prep);

  return pack;
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
  if (precision < 1)
    return;

  if (HVL_make_context(&m_hctx, precision) != HVL_OK)
    throw HVLLibException();

  m_precision = precision;
}
