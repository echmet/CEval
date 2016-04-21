#ifndef ABSTRACTPEAKFINDER_H
#define ABSTRACTPEAKFINDER_H

#include <memory>
#include <QPoint>
#include <QVector>

class PeakFinderResults {
public:
  explicit PeakFinderResults();
  PeakFinderResults(const PeakFinderResults &other);
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

template<typename Derived>
class AbstractPeakFinder
{
public:
  class AbstractParameters {
  public:
    virtual ~AbstractParameters() {}
  protected:
    AbstractParameters() {}
  };

  static PeakFinderResults find(const AbstractParameters &p) throw(std::bad_alloc)
  {
    if (s_me == nullptr)
      initialize();

    try {
      return s_me->findInternal(p);
    } catch (std::bad_cast) {
      return PeakFinderResults();
    }
  }


protected:
  virtual PeakFinderResults findInternal(const AbstractParameters &ap) throw (std::bad_cast) = 0;

  static AbstractPeakFinder<Derived> *s_me;

private:
  static void initialize() throw(std::bad_alloc)
  {
    s_me = new Derived();
  }

};

template<typename Derived>
AbstractPeakFinder<Derived> *AbstractPeakFinder<Derived>::s_me = nullptr;

#endif // ABSTRACTPEAKFINDER_H
