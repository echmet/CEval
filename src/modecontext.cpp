#include "modecontext.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_zoomer.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_widget.h"
#include "gui/adjustplotvisualsdialog.h"
#include "doubletostringconvertor.h"
#include "ploteventfilter.h"

ModeContext::ModeContext(QwtPlot *plot, QwtPlotPicker *picker, QwtPlotZoomer *zoomer, QObject *parent) :
  QObject(parent),
  m_active(false),
  m_plot(plot),
  m_plotPicker(picker),
  m_plotZoomer(zoomer),
  m_eventFilter(new PlotEventFilter(this))
{
}

ModeContext::~ModeContext()
{
  m_plot->canvas()->removeEventFilter(m_eventFilter);

  delete m_eventFilter;
}

void ModeContext::activate()
{
  for (std::shared_ptr<QwtPlotCurve> curve : m_plotCurves)
    curve->attach(m_plot);

  m_active = true;

  for (QMap<SerieProperties::Axis, QString>::const_iterator cit = m_axisTitles.cbegin();
       cit != m_axisTitles.cend(); cit++) {
    setAxisTitle(cit.key(), cit.value(), false);
  }

  m_plot->setTitle(m_plotTitle);

  for (const SerieProperties::Axis a : m_axisFont.keys())
    setAxisFont(a, m_axisFont[a]);

  m_plot->canvas()->installEventFilter(m_eventFilter);

  connect(m_plotPicker, static_cast<void (QwtPlotPicker::*)(const QPointF&)>(&QwtPlotPicker::selected), this, &ModeContext::onPointSelected);
  connect(m_eventFilter, &PlotEventFilter::mouseMoved, this, &ModeContext::onPointHovered);

  replot();
}

bool ModeContext::addSerie(const int id, const QString &title, SerieProperties::VisualStyle &style)
{
  std::shared_ptr<QwtPlotCurve> plotCurve;

  if (m_plotCurves.contains(id))
    return false;

  try {
    plotCurve = std::shared_ptr<QwtPlotCurve>(new QwtPlotCurve(title));
    m_plotCurves.insert(id, plotCurve);

    setSerieVisualStyle(id, style);
  } catch (std::bad_alloc&) {
    return false;
  }

  if (m_active)
    plotCurve->attach(m_plot);

  return true;
}

void ModeContext::adjustAppearance()
{
  AdjustPlotVisualsDialog dlg;
  int dlgRet;

  for (const int id : m_plotCurves.keys()) {
    bool ok;
    SerieProperties::VisualStyle style;
    std::shared_ptr<const QwtPlotCurve> curve = m_plotCurves[id];

    ok = serieVisualStyle(id, style);
    if (!ok)
      continue;

    AdjustPlotVisualsDialog::SerieVisuals sv(id);
    sv.visible = curve->isVisible();
    sv.lineColor = style.pen.color();
    sv.lineThickness = style.pen.widthF();
    sv.pointLineThickness = style.symbol()->pen().widthF();
    sv.pointColor = style.symbol()->pen().color();
    sv.pointFillColor = style.symbol()->brush().color();
    sv.pointSize = style.symbol()->size().width();
    sv.pointStyle = qwtSymbolStypeToPointStyle(style.symbol()->style());
    sv.lineStyle = qtPenStyleToLineStyle(style.pen.style());

    dlg.addSerieVisuals(curve->title().text(), sv);
  }

  for (const SerieProperties::Axis a : m_axisFont.keys()) {
    const QFont &f = m_axisFont[a];
    AdjustPlotVisualsDialog::AxisVisuals av(a);

    av.fontSize = f.pointSize();
    av.bold = f.bold();

    dlg.addAxisVisuals(SerieProperties::axisToName(a), av);
  }

  dlgRet = dlg.exec();
  if (dlgRet != QDialog::Accepted)
    return;

  QVector<AdjustPlotVisualsDialog::SerieVisuals> svVec = dlg.serieVisuals();
  for (const AdjustPlotVisualsDialog::SerieVisuals &sv : svVec) {
    if (!m_plotCurves.contains(sv.id))
      continue;

    std::shared_ptr<QwtPlotCurve> curve = m_plotCurves[sv.id];
    QPen p = curve->pen();

    const Qt::PenStyle ps = lineStyleToQtPenStyle(sv.lineStyle);
    p.setColor(sv.lineColor);
    p.setWidthF(sv.lineThickness);
    p.setStyle(ps);
    curve->setPen(p);

    const QwtSymbol *cSym = curve->symbol();
    const QwtSymbol::Style ss = static_cast<QwtSymbol::Style>(pointStyleToQwtSymbolStyle(sv.pointStyle));
    QwtSymbol *nSym = new QwtSymbol(ss);
    p = cSym->pen();
    p.setColor(sv.pointColor);
    p.setWidthF(sv.pointLineThickness);
    nSym->setBrush(QBrush(sv.pointFillColor));
    nSym->setPen(p);
    nSym->setSize(sv.pointSize);

    curve->setSymbol(nSym);

    curve->setVisible(sv.visible);
  }

  QVector<AdjustPlotVisualsDialog::AxisVisuals> avVec = dlg.axisVisuals();
  for (const AdjustPlotVisualsDialog::AxisVisuals &av : avVec) {
    SerieProperties::Axis a = av.a;

    QFont f = axisFont(a);

    f.setPointSize(av.fontSize);
    f.setBold(av.bold);

    setAxisFont(a, f);
  }

  m_plot->replot();
}

QFont ModeContext::axisFont(const SerieProperties::Axis a) const
{
  switch (a) {
  case SerieProperties::Axis::X_BOTTOM:
    return m_plot->axisFont(QwtPlot::Axis::xBottom);
    break;
  case SerieProperties::Axis::X_TOP:
    return m_plot->axisFont(QwtPlot::Axis::xTop);
    break;
  case SerieProperties::Axis::Y_LEFT:
    return m_plot->axisFont(QwtPlot::Axis::yLeft);
    break;
  case SerieProperties::Axis::Y_RIGHT:
    return m_plot->axisFont(QwtPlot::Axis::yRight);
    break;
  default:
    Q_ASSERT(false);
    return QFont();
  }
}

void ModeContext::clearAllSerieSamples()
{
  for (std::shared_ptr<QwtPlotCurve> curve : m_plotCurves)
    curve->setSamples(QVector<QPointF>());
}

void ModeContext::clearSerieSamples(const int id)
{
  if (!m_plotCurves.contains(id))
    return;

  m_plotCurves[id]->setSamples(QVector<QPointF>());
}

void ModeContext::deactivate()
{
  for (std::shared_ptr<QwtPlotCurve> curve : m_plotCurves)
    curve->detach();

  setAxisTitle(SerieProperties::Axis::X_BOTTOM, "", false);
  setAxisTitle(SerieProperties::Axis::X_TOP, "", false);
  setAxisTitle(SerieProperties::Axis::Y_LEFT, "", false);
  setAxisTitle(SerieProperties::Axis::Y_RIGHT, "", false);
  m_plot->setTitle("");
  m_active = false;

  m_boundingRect = QRectF();

  m_plot->canvas()->removeEventFilter(m_eventFilter);

  disconnect(m_plotPicker, static_cast<void (QwtPlotPicker::*)(const QPointF&)>(&QwtPlotPicker::selected), this, &ModeContext::onPointSelected);
  disconnect(m_eventFilter, &PlotEventFilter::mouseMoved, this, &ModeContext::onPointHovered);
}

void ModeContext::hideSerie(const int id)
{
  if (!m_plotCurves.contains(id))
    return;

  m_plotCurves[id]->detach();
}

Qt::PenStyle ModeContext::lineStyleToQtPenStyle(const AdjustPlotVisualsDialog::LineStyles ls) const
{
  switch (ls) {
  case AdjustPlotVisualsDialog::LineStyles::DASH:
    return Qt::DashLine;
    break;
  case AdjustPlotVisualsDialog::LineStyles::DASH_DOT:
    return Qt::DashDotLine;
    break;
  case AdjustPlotVisualsDialog::LineStyles::DASH_DOT_DOT:
    return Qt::DashDotDotLine;
    break;
  case AdjustPlotVisualsDialog::LineStyles::DOT:
    return Qt::DotLine;
    break;
  case AdjustPlotVisualsDialog::LineStyles::SOLID:
  default:
    return Qt::SolidLine;
    break;
  }
}

void ModeContext::onNumberFormatChanged(const QLocale *oldLocale)
{
  Q_UNUSED(oldLocale)

  m_plot->setLocale(DoubleToStringConvertor::locale());

  m_plot->setAxisScaleDraw(QwtPlot::Axis::xBottom, new QwtScaleDraw);
  m_plot->setAxisScaleDraw(QwtPlot::Axis::xTop, new QwtScaleDraw);
  m_plot->setAxisScaleDraw(QwtPlot::Axis::yLeft, new QwtScaleDraw);
  m_plot->setAxisScaleDraw(QwtPlot::Axis::yRight, new QwtScaleDraw);
}

void ModeContext::onPointHovered(const QPoint &pos)
{
  qreal x = m_plot->canvasMap(QwtPlot::Axis::xBottom).invTransform(pos.x());
  qreal y = m_plot->canvasMap(QwtPlot::Axis::yLeft).invTransform(pos.y());

  QPointF point(x, y);

  emit pointHovered(point, pos);
}

void ModeContext::onPointSelected(const QPointF &pos)
{
  QPoint cursor = QCursor::pos();
  m_plot->mapToGlobal(cursor);

  emit (pointSelected(pos, cursor));
}

int ModeContext::pointStyleToQwtSymbolStyle(const AdjustPlotVisualsDialog::PointStyles ps) const
{
  switch (ps) {
  case AdjustPlotVisualsDialog::PointStyles::CROSS:
    return QwtSymbol::Style::Cross;
    break;
  case AdjustPlotVisualsDialog::PointStyles::DIAMOND:
    return QwtSymbol::Style::Diamond;
    break;
  case AdjustPlotVisualsDialog::PointStyles::D_TRIANGLE:
    return QwtSymbol::Style::DTriangle;
    break;
  case AdjustPlotVisualsDialog::PointStyles::ELLIPSE:
    return QwtSymbol::Style::Ellipse;
    break;
  case AdjustPlotVisualsDialog::PointStyles::HEXAGON:
    return QwtSymbol::Style::Hexagon;
    break;
  case AdjustPlotVisualsDialog::PointStyles::H_LINE:
    return QwtSymbol::Style::HLine;
    break;
  case AdjustPlotVisualsDialog::PointStyles::L_TRIANGLE:
    return QwtSymbol::Style::LTriangle;
    break;
  case AdjustPlotVisualsDialog::PointStyles::NO_SYMBOL:
    return QwtSymbol::Style::NoSymbol;
    break;
  case AdjustPlotVisualsDialog::PointStyles::RECT:
    return QwtSymbol::Style::Rect;
    break;
  case AdjustPlotVisualsDialog::PointStyles::R_TRIANGLE:
    return QwtSymbol::Style::RTriangle;
    break;
  case AdjustPlotVisualsDialog::PointStyles::STAR_A:
    return QwtSymbol::Style::Star1;
    break;
  case AdjustPlotVisualsDialog::PointStyles::STAR_B:
    return QwtSymbol::Style::Star2;
    break;
  case AdjustPlotVisualsDialog::PointStyles::TRIANGLE:
    return QwtSymbol::Style::Triangle;
    break;
  case AdjustPlotVisualsDialog::PointStyles::U_TRIANGLE:
    return QwtSymbol::Style::UTriangle;
    break;
  case AdjustPlotVisualsDialog::PointStyles::V_LINE:
    return QwtSymbol::Style::VLine;
    break;
  case AdjustPlotVisualsDialog::PointStyles::X_CROSS:
    return QwtSymbol::Style::XCross;
    break;
  default:
    return QwtSymbol::Style::NoSymbol;
    break;
  }
}

AdjustPlotVisualsDialog::PointStyles ModeContext::qwtSymbolStypeToPointStyle(const int qwtSymbol) const
{
  switch (qwtSymbol) {
  case QwtSymbol::Style::NoSymbol:
    return AdjustPlotVisualsDialog::PointStyles::NO_SYMBOL;
    break;
  case QwtSymbol::Style::Ellipse:
    return AdjustPlotVisualsDialog::PointStyles::ELLIPSE;
    break;
  case QwtSymbol::Style::Rect:
    return AdjustPlotVisualsDialog::PointStyles::RECT;
    break;
  case QwtSymbol::Style::Diamond:
    return AdjustPlotVisualsDialog::PointStyles::DIAMOND;
    break;
  case QwtSymbol::Style::Triangle:
    return AdjustPlotVisualsDialog::PointStyles::TRIANGLE;
    break;
  case QwtSymbol::Style::DTriangle:
    return AdjustPlotVisualsDialog::PointStyles::D_TRIANGLE;
    break;
  case QwtSymbol::Style::UTriangle:
    return AdjustPlotVisualsDialog::PointStyles::U_TRIANGLE;
    break;
  case QwtSymbol::Style::LTriangle:
    return AdjustPlotVisualsDialog::PointStyles::L_TRIANGLE;
    break;
  case QwtSymbol::Style::RTriangle:
    return AdjustPlotVisualsDialog::PointStyles::R_TRIANGLE;
  case QwtSymbol::Style::Cross:
    return AdjustPlotVisualsDialog::PointStyles::CROSS;
    break;
  case QwtSymbol::Style::XCross:
    return AdjustPlotVisualsDialog::PointStyles::X_CROSS;
    break;
  case QwtSymbol::Style::HLine:
    return AdjustPlotVisualsDialog::PointStyles::H_LINE;
    break;
  case QwtSymbol::Style::VLine:
    return AdjustPlotVisualsDialog::PointStyles::V_LINE;
    break;
  case QwtSymbol::Style::Star1:
    return AdjustPlotVisualsDialog::PointStyles::STAR_A;
    break;
  case QwtSymbol::Style::Star2:
    return AdjustPlotVisualsDialog::PointStyles::STAR_B;
    break;
  case QwtSymbol::Style::Hexagon:
    return AdjustPlotVisualsDialog::PointStyles::HEXAGON;
    break;
  default:
    return AdjustPlotVisualsDialog::PointStyles::NO_SYMBOL;
    break;
  }
}

AdjustPlotVisualsDialog::LineStyles ModeContext::qtPenStyleToLineStyle(const int qtPenStyle) const
{
  switch (qtPenStyle) {
  case Qt::DashLine:
    return AdjustPlotVisualsDialog::LineStyles::DASH;
    break;
  case Qt::DashDotLine:
    return AdjustPlotVisualsDialog::LineStyles::DASH_DOT;
    break;
  case Qt::DashDotDotLine:
    return AdjustPlotVisualsDialog::LineStyles::DASH_DOT_DOT;
    break;
  case Qt::DotLine:
    return AdjustPlotVisualsDialog::LineStyles::DOT;
    break;
  case Qt::SolidLine:
  default:
    return AdjustPlotVisualsDialog::LineStyles::SOLID;
    break;
  }
}

QRectF ModeContext::range() const
{
  return uniteBoundingRects();
}

void ModeContext::removeSerie(const int id)
{
  if (!m_plotCurves.contains(id))
    return;

  std::shared_ptr<QwtPlotCurve> plotCurve = m_plotCurves[id];
  plotCurve->detach();

  m_plotCurves.remove(id);
}

void ModeContext::replot(const bool zoomOut)
{
  if (!m_active)
    return;

  const QRectF &currentZoom = m_plotZoomer->zoomRect();
  const QRectF &rect = uniteBoundingRects();

  if (m_boundingRect != rect) {
    double wm = rect.width() * 0.02;
    double hm = rect.height() * 0.02;

    if (wm == 0.0)
      wm = 1.0;
    if (hm == 0.0)
      hm = 1.0;

    m_plot->setAxisScale(QwtPlot::Axis::xBottom, rect.bottomLeft().x() - wm, rect.bottomRight().x() + wm);
    m_plot->setAxisScale(QwtPlot::Axis::yLeft, rect.topLeft().y() - hm, rect.bottomLeft().y() + hm);
  }
  m_plot->replot();
  setZoomBase(rect);

  if (!zoomOut) {
    m_plotZoomer->zoom(currentZoom);
  }
}

bool ModeContext::serieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  if (!m_plotCurves.contains(id))
    return false;

  QwtPlotCurve &c = *m_plotCurves[id];

  style.curveStyle = c.style();
  style.setSymbol(new QwtSymbol(c.symbol()->style(), c.symbol()->brush(), c.symbol()->pen(), c.symbol()->size()));
  style.pen = c.pen();
  style.visible = c.isVisible();

  return true;
}

void ModeContext::setAxisTitle(const SerieProperties::Axis axis, const QString &title, bool store)
{
  if (store)
    m_axisTitles[axis] = title;

  switch (axis) {
  case SerieProperties::Axis::X_BOTTOM:
    m_plot->setAxisTitle(QwtPlot::Axis::xBottom, title);
    break;
  case SerieProperties::Axis::X_TOP:
    m_plot->setAxisTitle(QwtPlot::Axis::xTop, title);
    break;
  case SerieProperties::Axis::Y_LEFT:
    m_plot->setAxisTitle(QwtPlot::Axis::yLeft, title);
    break;
  case SerieProperties::Axis::Y_RIGHT:
    m_plot->setAxisTitle(QwtPlot::Axis::yRight, title);
    break;
  default:
    break;
  }
}

void ModeContext::setAxisFont(const SerieProperties::Axis axis, const QFont &f)
{
  switch (axis) {
  case SerieProperties::Axis::X_BOTTOM:
    m_plot->setAxisFont(QwtPlot::Axis::xBottom, f);
    setAxisTitleFont(QwtPlot::Axis::xBottom, f);
    break;
  case SerieProperties::Axis::X_TOP:
    m_plot->setAxisFont(QwtPlot::Axis::xTop, f);
    setAxisTitleFont(QwtPlot::Axis::xTop, f);
    break;
  case SerieProperties::Axis::Y_LEFT:
    m_plot->setAxisFont(QwtPlot::Axis::yLeft, f);
    setAxisTitleFont(QwtPlot::Axis::yLeft, f);
    break;
  case SerieProperties::Axis::Y_RIGHT:
    m_plot->setAxisFont(QwtPlot::Axis::yRight, f);
    setAxisTitleFont(QwtPlot::Axis::yRight, f);
    break;
  default:
    Q_ASSERT(false);
    break;
  }

  m_axisFont[axis] = f;
}

void ModeContext::setAxisTitleFont(const int a, const QFont &f)
{
  QFont tf(f);
  tf.setBold(true);
  QwtText t = m_plot->axisTitle(a);
  t.setFont(tf);
  m_plot->setAxisTitle(a, t);
}

void ModeContext::setSerieSamples(const int id, const QVector<QPointF> &samples)
{
  if (!m_plotCurves.contains(id))
    return;

  m_plotCurves[id]->setSamples(samples);
}

void ModeContext::setPlotTitle(const QString &title)
{
  m_plotTitle = title;
  m_plot->setTitle(title);
}

void ModeContext::setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  if (!m_plotCurves.contains(id))
    return;

  std::shared_ptr<QwtPlotCurve> plotCurve = m_plotCurves[id];

  plotCurve->setSymbol(style.symbol());
  style.setSymbolOwned();
  plotCurve->setStyle(style.curveStyle);
  plotCurve->setPen(style.pen);
  plotCurve->setVisible(style.visible);
}

void ModeContext::setZoomBase(const QRectF &rect)
{
  if (m_boundingRect != rect) {
    m_plotZoomer->setZoomBase(rect);
    m_boundingRect = rect;
  }
}

void ModeContext::showSerie(const int id)
{
  if (!m_plotCurves.contains(id))
    return;

  m_plotCurves[id]->attach(m_plot);
}

QRectF ModeContext::uniteBoundingRects() const
{
  QRectF total;

  for (const std::shared_ptr<QwtPlotCurve> plotCurve : m_plotCurves) {
    if (plotCurve->data()->size() == 0)
      continue;
    if (plotCurve->plot() == nullptr)
      continue;

    const QRectF &rect = plotCurve->data()->boundingRect();
    total = total.united(rect);
  }

  return total;
}
