#ifndef ABSTRACTPEAKFINDER_H
#define ABSTRACTPEAKFINDER_H

#include <memory>
#include <QPoint>
#include <QVector>

class PeakFinderResults {
public:
  class Result {
  public:
    explicit Result();
    explicit Result(const int fromIndex, const int toIndex,
                    const double peakFromX, const double peakFromY,
                    const double peakToX, const double peakToY);
    Result(const Result &other);
    virtual ~Result();
    bool isValid() const;

    const int fromIndex;
    const int toIndex;

    const double peakFromX;
    const double peakFromY;
    const double peakToX;
    const double peakToY;

  private:
    const bool m_valid;

  };

  explicit PeakFinderResults();
  explicit PeakFinderResults(const QVector<std::shared_ptr<Result>> &results);
  PeakFinderResults(const PeakFinderResults &other);
  virtual ~PeakFinderResults();
  PeakFinderResults * copy() const; /* TODO: Fix derived classes */
  PeakFinderResults & operator=(const PeakFinderResults &other); /* TODO: Same here */

  const QVector<std::shared_ptr<Result>> results;

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

  static std::shared_ptr<PeakFinderResults> find(const AbstractParameters &p) noexcept(false)
  {
    if (s_me == nullptr)
      initialize();

    try {
      return s_me->findInternal(p);
    } catch (std::bad_cast &) {
      return std::shared_ptr<PeakFinderResults>(new PeakFinderResults());
    }
  }


protected:
  virtual std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) noexcept(false) = 0;

  static AbstractPeakFinder<Derived> *s_me;

private:
  static void initialize() noexcept(false)
  {
    s_me = new Derived();
  }

};

template<typename Derived>
AbstractPeakFinder<Derived> *AbstractPeakFinder<Derived>::s_me = nullptr;

#endif // ABSTRACTPEAKFINDER_H
