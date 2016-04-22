#ifndef PLOTEVENTFILTER_H
#define PLOTEVENTFILTER_H

#include <QObject>


class PlotEventFilter : public QObject
{
  Q_OBJECT
public:
  explicit PlotEventFilter(QObject *parent = nullptr);

protected:
  bool eventFilter(QObject *o, QEvent *evt);

signals:
  void mouseMoved(const QPoint &pos);

public slots:
};

#endif // PLOTEVENTFILTER_H
