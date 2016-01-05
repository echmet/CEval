#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#include <QPointF>
#include <QVector>
#include <memory>
#include "evaluationparametersitems.h"
#include "gui/selectpeakdialog.h"
#include "math/extremesearching.h"

class PeakFinder
{
public:
  class Parameters {
  public:
    explicit Parameters() = delete;
    Parameters(const QVector<QPointF> &data);

    const QVector<QPointF> &data;
    /* Autos */
    bool autoFrom;
    bool autoNoise;
    bool autoPeakFromX;
    bool autoPeakFromY;
    bool autoPeakHeight;
    bool autoPeakX;
    bool autoPeakToX;
    bool autoPeakToY;
    bool autoPeakWidth;
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
    double peakFromX;
    double peakFromY;
    double peakHeight;
    double peakX;
    double peakToX;
    double peakToY;
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
  };

  class Results {
  public:
    explicit Results();
    Results(const Results &other);
    bool isValid() const;
    void validate();

    long fromIndex;
    long toIndex;
    long indexAtMax;
    long fromPeakIndex;
    long toPeakIndex;

    long tPiCoarse;

    double maxY;
    double minY;
    double peakFromX;
    double peakFromY;
    double peakToX;
    double peakToY;
    double peakX;
    double peakHeight;
    double peakHeightBaseline;
    double noiseRefPoint;
    double slopeRefPoint;
    double twPLeft;
    double twPRight;
    double baselineSlope;
    double baselineIntercept;
    double noise;
    double slopeThreshold;
    double slopeWindow;

    std::shared_ptr<QVector<QPointF>> seriesA;
    std::shared_ptr<QVector<QPointF>> seriesB;

  private:
    bool m_valid;

  };

  PeakFinder() = delete;

  static Results find(const Parameters &p, SelectPeakDialog *selPeakDialog, const long inTPiCoarse = -1);

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
