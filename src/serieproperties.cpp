#include "serieproperties.h"

SerieProperties::VisualStyle::VisualStyle(const QPen pen, QwtSymbol *symbol, const QwtPlotCurve::CurveStyle curveStyle) :
  m_symbol(symbol),
  curveStyle(curveStyle),
  pen(pen),
  m_symbolOwned(false)
{
}

SerieProperties::VisualStyle::~VisualStyle()
{
  if (!m_symbolOwned)
    delete m_symbol;
}

QwtSymbol *SerieProperties::VisualStyle::symbol()
{
  return m_symbol;
}

void SerieProperties::VisualStyle::setSymbol(QwtSymbol *s)
{
  if (!m_symbolOwned)
    delete m_symbol;

  m_symbol = s;
  m_symbolOwned = false;
}

void SerieProperties::VisualStyle::setSymbolOwned()
{
  m_symbolOwned = true;
}

QString SerieProperties::axisToName(const Axis a)
{
  switch (a) {
  case Axis::X_BOTTOM:
    return QObject::tr("X bottom");
    break;
  case Axis::X_TOP:
    return QObject::tr("X top");
    break;
  case Axis::Y_LEFT:
    return QObject::tr("Y left");
    break;
  case Axis::Y_RIGHT:
    return QObject::tr("Y right");
    break;
  default:
    return "";
  }
}

QwtSymbol *SerieProperties::SQwtSymbol(const QwtSymbol::Style style, const qreal size, const QColor &color,
                                       const qreal lineThickness, const QBrush fill)
{
  QwtSymbol *s = new QwtSymbol(style);

  QPen p;
  p.setColor(color);
  p.setWidthF(lineThickness);
  s->setPen(p);
  s->setSize(size, size);
  s->setBrush(fill);

  return s;
}
