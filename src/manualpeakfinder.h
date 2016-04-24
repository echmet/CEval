#ifndef MANUALPEAKFINDER_H
#define MANUALPEAKFINDER_H

#include "abstractpeakfinder.h"

class ManualPeakFinder : public AbstractPeakFinder<ManualPeakFinder>
{
public:
  class Parameters : public AbstractParameters {
  public:
    explicit Parameters() = delete;
    Parameters(const QVector<QPointF> &data);

    const QVector<QPointF> &data;
    double fromX;
    double fromY;
    double toX;
    double toY;
  };

protected:
  std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) throw(std::bad_cast, std::bad_alloc) override;

};

#endif // MANUALPEAKFINDER_H
