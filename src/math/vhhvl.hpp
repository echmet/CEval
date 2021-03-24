#ifndef VHHVL_HPP
#define VHHVL_HPP

#include <array>
#include <cmath>

#define sgn(v) ((v > 0) - (v < 0))

template <typename Real>
class VHHVLCalculator {
  static constexpr Real DELTA = 1e-9;
  static constexpr Real TWO_DELTA = 2.0 * DELTA;

  static constexpr Real C_PI = 3.14159265358979323846;
  static constexpr Real C_SQRT2 = 1.41421356237309504880;
  static constexpr Real C_LN2 = 0.693147180559945309417;
  static constexpr Real C_HALF_LNPI = 0.5723649429247001;

  static constexpr Real GAMMA_THR = 25.0;
  static constexpr Real E_THR = 50.0;

  static
  Real alpha(double t, double a1, double a2)
  {
    return (t - a1) / (C_SQRT2 * a2);
  }

  static
  Real beta(Real a, double a3d)
  {
    return -sgn(a3d) * a;
  }

  static
  Real gamma(Real b)
  {
    if (b > GAMMA_THR) {
        Real bb = b*b;
        return -bb - C_HALF_LNPI + (Real(5.0) - Real(4.0) * b)/(Real(8.0) * bb);
    }
    return std::log(std::erfc(b));
  }

  static
  Real Q(Real b, Real apsi, Real lepsi)
  {
    return -C_LN2 + lepsi + apsi + gamma(b);
  }

  static
  Real E(Real b, Real q, Real apsi, Real lepsi)
  {
    Real p = -b*b + apsi + lepsi;
    if (q > E_THR)
        return p - q;
    return p - std::log(Real(1.0) + std::exp(q));
  }

  static
  Real V(double t, double a1, double a2, double a3d)
  {
    Real apsi = std::abs(a3d);

    if (apsi < Real(0.00001)) {
        Real n = (t - a1) / a2;
        Real nn = n * n;
        return std::exp(Real(-0.5) * nn);
    } else {
        Real lepsi = std::log(Real(1.0) - std::exp(-apsi));
        Real a = alpha(t, a1, a2);
        Real b = beta(a, a3d);
        Real q = Q(b, apsi, lepsi);
        Real e = E(b, q, apsi, lepsi);

        return Real(1.0)/apsi * expl(e);
    }
}

  static
  Real K(double a0, double a2)
  {
    return a0 / (std::sqrt(Real(2) * C_PI) * a2);
  }

public:
  class Pack {
  public:
    std::array<Real, 6> results;

    Real & y() { return results[0]; }
    Real & dt() { return results[1]; }
    Real & da0() { return results[2]; }
    Real & da1() { return results[3]; }
    Real & da2() { return results[4]; }
    Real & da3d() { return results[5]; }

    const Real & y() const { return results[0]; };
    const Real & dt() const { return results[1]; }
    const Real & da0() const { return results[2]; }
    const Real & da1() const { return results[3]; }
    const Real & da2() const { return results[4]; }
    const Real & da3d() const { return results[5]; }

  };

  static
  Real y(double t, double a0, double a1, double a2, double a3d)
  {
    Real k = K(a0, a2);
    return k * V(t, a1, a2, a3d);
  }

  static
  Real dt(double t, double a0, double a1, double a2, double a3d)
  {
    Real k = K(a0, a2);

    Real lo = V(t - DELTA, a1, a2, a3d);
    Real hi = V(t + DELTA, a1, a2, a3d);

    return k * (hi - lo) / TWO_DELTA;
  }

  static
  Real da0(double t, double a0, double a1, double a2, double a3d)
  {
    Real v = V(t, a1, a2, a3d);
    Real lo = K(a0 - DELTA, a2);
    Real hi = K(a0 + DELTA, a2);

    return v * (hi - lo) / TWO_DELTA;
  }

  static
  Real da1(double t, double a0, double a1, double a2, double a3d)
  {
    Real k = K(a0, a2);

    Real lo = V(t, a1 - DELTA, a2, a3d);
    Real hi = V(t, a1 + DELTA, a2, a3d);

    return k * (hi - lo) / TWO_DELTA;
  }

  static
  Real da2(double t, double a0, double a1, double a2, double a3d)
  {
    Real lo = K(a0, a2 - DELTA) * V(t, a1, a2 - DELTA, a3d);
    Real hi = K(a0, a2 + DELTA) * V(t, a1, a2 + DELTA, a3d);

    return (hi - lo) / TWO_DELTA;
  }

  static
  Real da3d(double t, double a0, double a1, double a2, double a3d)
  {
    Real k = K(a0, a2);

    Real lo = V(t, a1, a2, a3d - DELTA);
    Real hi = V(t, a1, a2, a3d + DELTA);

    return k * (hi - lo) / TWO_DELTA;
  }

  static
  Pack everything(double t, double a0, double a1, double a2, double a3d)
  {
    Real k = K(a0, a2);
    Real v = V(t, a1, a2, a3d);
    Real dt_lo;
    Real dt_hi;
    Real da0_lo;
    Real da0_hi;
    Real da1_lo;
    Real da1_hi;
    Real da2_lo;
    Real da2_hi;
    Real da3d_lo;
    Real da3d_hi;
    Pack result;

    result.y() = k * v;

    /* dt */
    dt_lo = V(t - DELTA, a1, a2, a3d);
    dt_hi = V(t + DELTA, a1, a2, a3d);

    result.dt() = k * (dt_hi - dt_lo) / TWO_DELTA;

    /* da0 */
    da0_lo = K(a0 - DELTA, a2);
    da0_hi = K(a0 + DELTA, a2);

    result.da0() = v * (da0_hi - da0_lo) / TWO_DELTA;

    /* da1 */
    da1_lo = V(t, a1 - DELTA, a2, a3d);
    da1_hi = V(t, a1 + DELTA, a2, a3d);

    result.da1() = k * (da1_hi - da1_lo) / TWO_DELTA;

    /* da2 */
    da2_lo = K(a0, a2 - DELTA) * V(t, a1, a2 - DELTA, a3d);
    da2_hi = K(a0, a2 + DELTA) * V(t, a1, a2 + DELTA, a3d);

    result.da2() = (da2_hi - da2_lo) / TWO_DELTA;

    /* da3 */
    da3d_lo = V(t, a1, a2, a3d - DELTA);
    da3d_hi = V(t, a1, a2, a3d + DELTA);

    result.da3d() = k * (da3d_hi - da3d_lo) / TWO_DELTA;

    return result;
  }
};
#endif // VHHVL_HPP
