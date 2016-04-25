#ifndef HELPERS_H
#define HELPERS_H

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

  static qreal maxYValue(const QVector<QPointF> &v)
  {
    Q_ASSERT(v.length() > 0);

    qreal max = v.at(0).y();
    for (int idx = 1; idx < v.length(); idx++) {
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

  static qreal minYValue(const QVector<QPointF> &v)
  {
    Q_ASSERT(v.length() > 0);

    qreal max = v.at(0).y();
    for (int idx = 1; idx < v.length(); idx++) {
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

  static qreal yForX(const qreal x, const QVector<QPointF> &v) {
    int idx = 0;

    if (v.size() == 0)
      throw std::out_of_range("Vector contains no data");

    if (v.at(0).x() > x)
      throw std::out_of_range("Value of X is lower than the lowest X value in the vector");

    while (idx < v.size()) {
      if (v.at(idx).x() >= x)
        break;

      idx++;
    }

    if (idx >= v.size())
      throw std::out_of_range("Value of X is higher than the highest X value in the vector");

    return v.at(idx).y();
  }

  static void execCFIT();
  static bool exitApplicationWithWarning();
  static QString hvlstrToQString(void *str);
};

#endif // HELPERS_H

