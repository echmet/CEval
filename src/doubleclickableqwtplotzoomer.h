#ifndef DOUBLECLICKABLEQWTPLOTZOOMER_H
#define DOUBLECLICKABLEQWTPLOTZOOMER_H

#include "qwt_plot_zoomer.h"

class DoubleClickableQwtPlotZoomer : public QwtPlotZoomer
{
public:
  explicit DoubleClickableQwtPlotZoomer(QWidget *parent = nullptr);

  virtual void widgetMouseDoubleClickEvent(QMouseEvent *evt) override;
};

#endif // DOUBLECLICKABLEQWTPLOTZOOMER_H
