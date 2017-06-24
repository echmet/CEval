#include <QWidget>
#include "scrollareaeventfilter.h"

ScrollAreaEventFilter::ScrollAreaEventFilter(std::function<void (QObject *, QEvent *)> todo, QWidget *w, QObject *parent) :
  QObject(parent),
  m_todo(todo)
{
  w->installEventFilter(this);
}

bool ScrollAreaEventFilter::eventFilter(QObject *object, QEvent *ev)
{
  Q_UNUSED(object);

  m_todo(object, ev);

  return false;
}
