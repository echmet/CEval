#include "ploteventfilter.h"
#include <QMouseEvent>

PlotEventFilter::PlotEventFilter(QObject *parent) : QObject(parent)
{
}

bool PlotEventFilter::eventFilter(QObject *o, QEvent *evt)
{
  Q_UNUSED(o)

  if (evt->type() == QEvent::MouseMove) {
    QMouseEvent *mevt = static_cast<QMouseEvent *>(evt);

    emit mouseMoved(mevt->pos());
  }

  return false;
}
