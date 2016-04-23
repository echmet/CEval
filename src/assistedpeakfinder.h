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

protected:
  virtual PeakFinderResults findInternal(const AbstractParameters &ap) throw(std::bad_cast) override;

private:
  enum EState {stTop = 0, stBeforeInflex = 1, stAfterInflex = 2, stBottom = 3};

  static bool checkBounds(const unsigned long i, const QVector<QPointF> &data);

  class TSearchHandler : public TExtremeSearcher::TDataHandler {
  public:
    QVector<QPointF> m_Data;
    long m_Begin;
    long m_End;

    TSearchHandler(const QVector<QPointF> &Data, long Begin = 0, long End = -1);
    TSearchHandler(TSearchHandler &T);

    virtual double GetX(long I);
    virtual double GetY(long I);
    virtual long Count();
  };

  class TPeaksSearcher : public TExtremeSearcher {
  public:
    typedef std::vector<long> container_type;
    container_type Extremes;

    TPeaksSearcher(TSearchHandler *S, long ChainPoints, double Noise, bool LeftBoundary = false, bool RightBoundary = false);
    TPeaksSearcher(TPeaksSearcher &T);
    virtual void Search();

  protected:
    virtual void OnMaximum(double Value, long Index);
    virtual void OnMinimum(double Value, long Index);
  };

};

#endif // PEAKFINDER_H
