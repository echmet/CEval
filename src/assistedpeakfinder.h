#ifndef ASSISTEDPEAKFINDER_H
#define ASSISTEDPEAKFINDER_H

#include "abstractpeakfinder.h"
#include "evaluationparametersitems.h"
#include "gui/selectpeakdialog.h"
#include "math/extremesearching.h"


class AssistedPeakFinder : public AbstractPeakFinder<AssistedPeakFinder>
{
public:
  class Parameters : public AbstractParameters {
  public:
    explicit Parameters() = delete;
    Parameters(const QVector<QPointF> &data);
    ~Parameters() override;

    const QVector<QPointF> &data;
    /* Autos */
    bool autoFrom;
    bool autoNoise;
    bool autoSlopeRefPoint;
    bool autoSlopeThreshold;
    bool autoSlopeWindow;
    bool autoTo;

    /* Bools */
    bool noiseCorrection;
    bool disturbanceDetection;

    /* Numeric */
    double from;
    double noise;
    double noiseCoefficient;
    double noisePoint;
    double noiseWindow;
    double peakWindow;
    double slopeRefPoint;
    double slopeSensitivity;
    double slopeThreshold;
    double slopeWindow;
    double to;
    double disturbanceDetectionWindow;

    /* Constrained */
    EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm;
    EvaluationParametersItems::ComboShowWindow showWindow;
    EvaluationParametersItems::ComboWindowUnits windowUnits;

    /* Additional data needed by the algorithm */
    SelectPeakDialog *selPeakDialog;
    long inTPiCoarse;
  };

  class AssistedPeakFinderResult : public PeakFinderResults::Result {
  public:
    explicit AssistedPeakFinderResult();
    explicit AssistedPeakFinderResult(const PeakFinderResults::Result &r,
                                      const double noiseRefPoint, const double slopeRefPoint,
                                      const double noise,
                                      const double slopeThreshold, const double slopeWindow,
                                      const std::shared_ptr<QVector<QPointF>> &seriesA,
                                      const std::shared_ptr<QVector<QPointF>> &seriesB);
    AssistedPeakFinderResult(const AssistedPeakFinderResult &other);
    virtual ~AssistedPeakFinderResult();

    const double noiseRefPoint;
    const double slopeRefPoint;
    const double noise;
    const double slopeThreshold;
    const double slopeWindow;

    std::shared_ptr<const QVector<QPointF>> seriesA;
    std::shared_ptr<const QVector<QPointF>> seriesB;

  };

protected:
  virtual std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) noexcept(false) override;

private:
  enum EState {stTop = 0, stBeforeInflex = 1, stAfterInflex = 2, stBottom = 3};

  static bool checkBounds(const int i, const QVector<QPointF> &data);
  std::shared_ptr<AssistedPeakFinderResult> findStageTwo(const Parameters &p,
                                                         const QVector<QPointF> &Data,
                                                         const int _tPi,
                                                         std::shared_ptr<QVector<QPointF>> &seriesA, std::shared_ptr<QVector<QPointF>> &seriesB,
                                                         const int _tAi,
                                                         const int tBEGi, const int tENDi,
                                                         const double XMin,
                                                         const double Noise_2, const double ppm,
                                                         const double SlopeSensitivity, const double SlopeThreshold,
                                                         const int NoiseWindow, const double Noise,
                                                         const int PeakWindow,
                                                         const double tnrp, const double tsrp);

  class TSearchHandler : public TExtremeSearcher::TDataHandler {
  public:
    QVector<QPointF> m_Data;
    int m_Begin;
    int m_End;

    TSearchHandler(const QVector<QPointF> &Data, int Begin = 0, int End = -1);
    TSearchHandler(TSearchHandler &T);

    virtual double GetX(int I);
    virtual double GetY(int I);
    virtual int Count();
  };

  class TPeaksSearcher : public TExtremeSearcher {
  public:
    typedef std::vector<long> container_type;
    container_type Extremes;

    TPeaksSearcher(TSearchHandler *S, int ChainPoints, double Noise, bool LeftBoundary = false, bool RightBoundary = false);
    TPeaksSearcher(TPeaksSearcher &T);
    virtual void Search();

  protected:
    virtual void OnMaximum(double Value, int Index);
    virtual void OnMinimum(double Value, int Index);
  };

  int chopLeadingDisturbance(const QVector<QPointF> &data, const int fromIdx, const int toIdx);
};

#endif // PEAKFINDER_H
