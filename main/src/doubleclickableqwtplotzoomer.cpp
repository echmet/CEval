#include "doubleclickableqwtplotzoomer.h"
#include <QMouseEvent>

DoubleClickableQwtPlotZoomer::DoubleClickableQwtPlotZoomer(QWidget *parent) :
  QwtPlotZoomer(parent)
{
  setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
}

void DoubleClickableQwtPlotZoomer::widgetMouseDoubleClickEvent(QMouseEvent *evt)
{
  if (evt->button() == Qt::LeftButton)
    zoom(0);
}
