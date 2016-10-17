#ifndef HELPERS_H
#define HELPERS_H

#include <cmath>
#include <QPointF>
#include <QVector>
#include <QWidget>

class Helpers
{
public:
  template<typename T>
  static T *getRootWidget(QWidget *child)
  {
    /* I know and shut up! */
    T *rootWidget;
    QWidget *w = child;

    while ((rootWidget = qobject_cast<T *>(w)) == nullptr) {
      if (w == nullptr)
        return nullptr;

      w = w->parentWidget();
    }

    return rootWidget;
  }

  template<typename T>
  static T maxValue(const QVector<T> &v)
  {
    Q_ASSERT(v.length() > 0);

    T max = v.at(0);
    for (int idx = 1; idx < v.length(); idx++) {
      if (max < v.at(idx))
        max = v.at(idx);
    }

    return max;
  }

  static qreal maxXValue(const QVector<QPointF> &v)
  {
    Q_ASSERT(v.length() > 0);

    qreal max = v.at(0).x();
    for (int idx = 1; idx < v.length(); idx++) {
      if (max < v.at(idx).x())
        max = v.at(idx).x();
    }

    return max;
  }

  static qreal maxYValue(const QVector<QPointF> &v, const int from = 0, int to = -1)
  {
    Q_ASSERT(v.length() > 0);

    if (to < 0)
      to = v.length() - 1;

    Q_ASSERT(to < v.length());
    Q_ASSERT(from < to);

    qreal max = v.at(from).y();
    for (int idx = from + 1; idx < to; idx++) {
      if (max < v.at(idx).y())
        max = v.at(idx).y();
    }

    return max;
  }

  template<typename T>
  static T minValue(const QVector<T> &v)
  {
    Q_ASSERT(v.length() > 0);

    T max = v.at(0);
    for (int idx = 1; idx < v.length(); idx++) {
      if (max > v.at(idx))
        max = v.at(idx);
    }

    return max;
  }

  static qreal minXValue(const QVector<QPointF> &v)
  {
    Q_ASSERT(v.length() > 0);

    qreal max = v.at(0).x();
    for (int idx = 1; idx < v.length(); idx++) {
      if (max > v.at(idx).x())
        max = v.at(idx).x();
    }

    return max;
  }

  static qreal minYValue(const QVector<QPointF> &v, const int from = 0, int to = -1)
  {
    Q_ASSERT(v.length() > 0);

    if (to < 0)
      to = v.length() - 1;

    Q_ASSERT(to < v.length());
    Q_ASSERT(from < to);

    qreal max = v.at(from).y();
    for (int idx = from + 1; idx <= to; idx++) {
      if (max > v.at(idx).y())
        max = v.at(idx).y();
    }

    return max;
  }

  template<typename T>
  static bool isSensible(const T t)
  {
    return std::isfinite(t) && (t > 0.0);
  }

  static qreal yForX(const qreal x, const QVector<QPointF> &v);
  static void execCFIT();
  static bool exitApplicationWithWarning();
};

#endif // HELPERS_H

