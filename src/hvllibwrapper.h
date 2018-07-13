#ifndef HVLLIBWRAPPER_H
#define HVLLIBWRAPPER_H

#include <QList>
#include <array>

struct HVL_Context;

class HVLLibWrapper {
public:
  class HVLLibException : public std::exception {
  public:
    explicit HVLLibException();
    const char * what() const noexcept;

  };

  enum class Parameter : size_t {
    T = 0,
    DX = 1,
    DA0 = 2,
    DA1 = 3,
    DA2 = 4,
    DA3 = 5
  };

  enum ParameterFlags : int {
    T = 0x1,
    DX = 0x2,
    DA0 = 0x4,
    DA1 = 0x8,
    DA2 = 0x10,
    DA3 = 0x20
  };

  class XY {
  public:
    explicit XY();
    explicit XY(const double X, const double Y);

    const double x;
    const double y;

    XY & operator=(const XY &other);
  };

  class XYPack {
  public:
    void setData(const Parameter p, const XY &data);
    const XY & operator[](const Parameter p) const;
    const XY & operator[](const size_t paramIdx) const;

  private:
    typedef typename std::underlying_type<HVLLibWrapper::Parameter>::type PUType;
    PUType parameterToIndex(const Parameter p) const;

    std::array<XY, 6> m_data;

  };

  class Result {
  public:
    explicit Result();
    explicit Result(const QList<XY> &values);

    const XY & operator[](const int idx) const;
    int count() const;

  private:
    const QList<XY> m_values;
  };

  explicit HVLLibWrapper(const int precision);
  ~HVLLibWrapper();
  XY calculate(const Parameter p,
               const double x,
               const double a0, const double a1, const double a2, const double a3) const;
  const XYPack calculateMultiple(const ParameterFlags params, const double x, const double a0, const double a1, const double a2, const double a3) const;
  Result calculateRange(const Parameter p,
                        const double from, const double to, const double step,
                        const double a0, const double a1, const double a2, const double a3) const;
  int precision() const;
  void setPrecision(const int precision);

private:
  HVL_Context *m_hctx;
  int m_precision;

};

#endif // HVLLIBWRAPPER_H
