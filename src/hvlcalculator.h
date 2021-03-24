#ifndef HVLCALCULATOR_H
#define HVLCALCULATOR_H

#include <QVector>
#include <QPointF>
#include <QObject>
#include <QTime>

namespace echmet {
  namespace regressCore {
    template<typename XT, typename YT> class HVLPeak;
  }
}

class HVLCalculator {
public:
  class HVLParameters {
  public:
    enum class Failure {
        OK,
        ABORTED,
        CANNOT_INIT,
        NO_CONVERGENCE
    };

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
    int finalPrecision;
    Failure failure;

  private:
    bool m_valid;
  };

  class HVLInParameters {
  public:
    explicit HVLInParameters();

    const QVector<QPointF> *data;
    int fromIdx;
    int toIdx;
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

  static bool initialize();
  ~HVLCalculator();

  static void applyBaseline(QVector<QPointF> &data, const double k, const double q);

  static HVLParameters fit(
    const QVector<QPointF> &data, const int fromIdx, const int toIdx,
    const double a0, const double a1, const double a2, const double a3,
    const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
    const double bsl, const double bslSlope,
    const double epsilon, const int iterations,
    const bool showStats
  );

  static QVector<QPointF> plot(
    const double a0, const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step
  );


private:
  HVLCalculator() = default;
  void doFit(HVLParameters *out, const HVLInParameters *in);

  static HVLCalculator *s_me;

};

class HVLCalculatorWorker : public QObject {
public:
  explicit HVLCalculatorWorker(const HVLCalculator::HVLInParameters &params);
  ~HVLCalculatorWorker();
  const HVLCalculator::HVLParameters & results() const;

  void process();

private:
  echmet::regressCore::HVLPeak<double, double> *m_regressor;
  HVLCalculator::HVLParameters m_outParams;
  QTime m_fitStartTime;

  const HVLCalculator::HVLInParameters m_params;

};

#endif // HVLCALCULATOR_H
