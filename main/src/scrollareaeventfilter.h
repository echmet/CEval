#ifndef SCROLLAREAEVENTFILTER_H
#define SCROLLAREAEVENTFILTER_H

#include <QObject>
#include <functional>

class ScrollAreaEventFilter : public QObject
{
  Q_OBJECT
public:
  explicit ScrollAreaEventFilter(std::function<void (QObject *, QEvent *)> todo, QWidget *w, QObject *parent = nullptr);

private:
  bool eventFilter(QObject *object, QEvent *ev);

  std::function<void (QObject *, QEvent *)> m_todo;

signals:

public slots:
};

#endif // SCROLLAREAEVENTFILTER_H
