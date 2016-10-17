#ifndef HVLLIBWRAPPER_H
#define HVLLIBWRAPPER_H

#include <QList>

class HVL_Context;

class HVLLibWrapper {
public:
  class HVLLibException : public std::exception {
  public:
    explicit HVLLibException();
    const char * what() const noexcept;

  };

  enum class Parameter {
    T,
    DX,
    DA0,
    DA1,
    DA2,
    DA3
  };

  class XY {
  public:
    explicit XY(const double X, const double Y);

    const double x;
    const double y;

    XY & operator=(const XY &other);
  };

  class AllParameters {
  public:
    explicit AllParameters(const XY &da0, const XY &da1, const XY &da2, const XY &da3);

    const XY da0;
    const XY da1;
    const XY da2;
    const XY da3;
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
  AllParameters calculateAll(const double x, const double a0, const double a1, const double a2, const double a3) const;
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
