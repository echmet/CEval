#ifndef HVLFITTER_H
#define HVLFITTER_H

#include "math/matrix/fwdcl.h"
#include <QVector>
#include <QPointF>
#include <QObject>

namespace echmet {
namespace math {
  class HVL_dll;
}
}

class HVLCalculator : public QObject
{
  Q_OBJECT
public:
  class HVLParameters {
  public:
    explicit HVLParameters();
    explicit HVLParameters(const double a0, const double a1, const double a2, const double a3);
    bool isValid() const;
    void validate();

    double a0;
    double a1;
    double a2;
    double a3;
    double s;
    double s0;
    int iterations;

  private:
    bool m_valid;
  };

  class HVLInParameters {
  public:
    explicit HVLInParameters();

    const QVector<QPointF> *data;
    unsigned long fromIdx;
    unsigned long toIdx;
    double a0;
    double a1;
    double a2;
    double a3;
    bool a0fixed;
    bool a1fixed;
    bool a2fixed;
    bool a3fixed;
    double bsl;
    double bslSlope;
    double epsilon;
    int iterations;
  };

  static bool available();
  static bool initialize();
  ~HVLCalculator();

  static void applyBaseline(QVector<QPointF> &data, const double k, const double q);

  static HVLParameters fit(
    const QVector<QPointF> &data, const unsigned long fromIdx, const unsigned long toIdx,
    const double a0, const double a1, const double a2, const double a3,
    const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
    const double bsl, const double bslSlope,
    const double epsilon, const int iterations, const int digits,
    const bool showStats
  );

  static QVector<QPointF> plot(
    const double a0, const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step,  const int    digits
  );


private:
  explicit HVLCalculator(QObject *parent = nullptr);
  void doFit(HVLParameters *out, const HVLInParameters *in);

  echmet::math::HVL_dll *m_hvlLib;

  static HVLCalculator *s_me;

signals:
  void hvlFitDone();

};

#endif // HVLFITTER_H
