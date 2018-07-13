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
    ~Parameters() override;

    const QVector<QPointF> &data;
    double fromX;
    double fromY;
    double toX;
    double toY;
  };

protected:
  std::shared_ptr<PeakFinderResults> findInternal(const AbstractParameters &ap) noexcept(false) override;

};

#endif // MANUALPEAKFINDER_H
