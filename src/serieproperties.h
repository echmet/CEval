#ifndef SERIEPROPERTIES_H
#define SERIEPROPERTIES_H

#include "qwt_symbol.h"
#include "qwt_plot_curve.h"
#include <QPen>

class SerieProperties
{
public:
  SerieProperties() = delete;

  enum class Axis {
    X_BOTTOM,
    X_TOP,
    Y_LEFT,
    Y_RIGHT,
    INVALID
  };

  class VisualStyle {
  public:
    explicit VisualStyle(const QPen pen = QPen(QBrush(Qt::black, Qt::SolidPattern), 1.0),
                         QwtSymbol *symbol = new QwtSymbol(QwtSymbol::Style::NoSymbol),
                         const QwtPlotCurve::CurveStyle curveStyle = QwtPlotCurve::CurveStyle::Lines);
    ~VisualStyle();
    QwtPlotCurve::CurveStyle curveStyle;
    QPen pen;
    QwtSymbol *symbol();
    void setSymbol(QwtSymbol *s);
    void setSymbolOwned();

  private:
    bool m_symbolOwned;
    QwtSymbol *m_symbol;
  };

  static QString axisToName(const Axis a);
  static QwtSymbol *SQwtSymbol(const QwtSymbol::Style style, const qreal size, const QColor &color = Qt::black,
                               const qreal lineThickness = 1.0, const QBrush fill = QBrush(Qt::black));

};

#endif // SERIEPROPERTIES_H

