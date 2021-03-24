#ifndef HVLCALCULATOR_H
#define HVLCALCULATOR_H

#include <QVector>
#include <QPointF>
#include <QObject>
#include <QMutex>
#include <QTime>

namespace echmet {
  namespace regressCore {
    template<typename XT, typename YT> class HVLPeak;
  }
}

class HVLCalculator : public QObject
{
  Q_OBJECT
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

  class HVLEstimateParameters {
  public:
    explicit HVLEstimateParameters(const double from, const double to, const double step,
                                   const double a0, const double a1, const double a2, const double a3,
                                   const bool negative);

    const double from;
    const double to;
    const double step;
    const double a0;
    const double a1;
    const double a2;
    const double a3;
    const bool negative;

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

  static int estimatePrecision(const double from, const double to, const double step,
                               const double a0, const double a1, const double a2, const double a3,
                               const bool negative);

  static HVLParameters fit(
    const QVector<QPointF> &data, const int fromIdx, const int toIdx,
    const double a0, const double a1, const double a2, const double a3,
    const bool a0fixed, const bool a1fixed, const bool a2fixed, const bool a3fixed,
    const double bsl, const double bslSlope,
    const double epsilon, const int iterations, const int digits,
    const bool autoDigits,
    const bool showStats
  );

  static QVector<QPointF> plot(
    const double a0, const double a1, const double a2, const double a3,
    const double fromX, const double toX,
    const double step,  const int    digits
  );


private:
  explicit HVLCalculator(QObject *parent, const int precision);
  void doFit(HVLParameters *out, const HVLInParameters *in);

  static HVLCalculator *s_me;

};

class HVLCalculatorWorker : public QObject {
  Q_OBJECT
public:
  explicit HVLCalculatorWorker(const HVLCalculator::HVLInParameters &params);
  ~HVLCalculatorWorker();
  const HVLCalculator::HVLParameters & results() const;

signals:
  void finished();
  void nextIteration(const int iteration, const double avgTimePerIter);

public slots:
  void abort();
  void process();

private:
  echmet::regressCore::HVLPeak<double, double> *m_regressor;
  HVLCalculator::HVLParameters m_outParams;
  QTime m_fitStartTime;
  QMutex m_abortLock;
  bool m_aborted;

  const HVLCalculator::HVLInParameters m_params;

};

class HVLEstimatorWorker : public QObject {
  Q_OBJECT
public:
  explicit HVLEstimatorWorker(const HVLCalculator::HVLEstimateParameters &params);
  ~HVLEstimatorWorker();
  int precision() const;

signals:
  void finished();

public slots:
  void process();

private:
  const HVLCalculator::HVLEstimateParameters &m_params;
  int m_precision;

};

#endif // HVLCALCULATOR_H
