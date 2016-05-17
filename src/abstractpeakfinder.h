#ifndef ABSTRACTPEAKFINDER_H
#define ABSTRACTPEAKFINDER_H

#include <memory>
#include <QPoint>
#include <QVector>

class PeakFinderResults {
public:
  explicit PeakFinderResults();
  PeakFinderResults(const PeakFinderResults &other);
  virtual ~PeakFinderResults();
  virtual PeakFinderResults *copy() const;
  bool isValid() const;
  void validate();

  int fromIndex;
  int toIndex;

  double peakFromX;
  double peakFromY;
  double peakToX;
  double peakToY;

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

  virtual ~AbstractPeakFinder() {}

  static std::shared_ptr<PeakFinderResults> find(const AbstractParameters &p) throw(std::bad_alloc)
  {
    if (s_me == nullptr)
      initialize();

    try {
      return s_me->findInternal(p);
    } catch (std::bad_cast) {
      return std::shared_ptr<PeakFinderResults>(new PeakFinderResults());
    }
  }


protected:
  virtual std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) throw (std::bad_cast, std::bad_alloc) = 0;

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
