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
    double tEOF;
    double to;

    /* Constrained */
    EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm;
    EvaluationParametersItems::ComboShowWindow showWindow;
    EvaluationParametersItems::ComboWindowUnits windowUnits;

    /* Additional data needed by the algorithm */
    SelectPeakDialog *selPeakDialog;
    long inTPiCoarse;
  };

  class AssistedPeakFinderResults : public PeakFinderResults {
  public:
    explicit AssistedPeakFinderResults();
    AssistedPeakFinderResults(const AssistedPeakFinderResults &other);
    AssistedPeakFinderResults &operator=(const AssistedPeakFinderResults &other);
    virtual AssistedPeakFinderResults *copy() const override;

    double noiseRefPoint;
    double slopeRefPoint;
    double noise;
    double slopeThreshold;
    double slopeWindow;

    std::shared_ptr<QVector<QPointF>> seriesA;
    std::shared_ptr<QVector<QPointF>> seriesB;
  };

protected:
  virtual std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) noexcept(false) override;

private:
  enum EState {stTop = 0, stBeforeInflex = 1, stAfterInflex = 2, stBottom = 3};

  static bool checkBounds(const int i, const QVector<QPointF> &data);

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

};

#endif // PEAKFINDER_H
