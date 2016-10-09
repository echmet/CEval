#include "plotcontextlimited.h"
#include "plotcontext.h"

PlotContextLimited::PlotContextLimited(std::shared_ptr<PlotContext> modeCtx, QObject *parent) :
  QObject(parent),
  m_modeCtx(modeCtx)
{
  connect(m_modeCtx.get(), &PlotContext::pointHovered, this, &PlotContextLimited::onPointHovered);
  connect(m_modeCtx.get(), &PlotContext::pointSelected, this, &PlotContextLimited::onPointSelected);
}

bool PlotContextLimited::addSerie(const int id, const QString &title, SerieProperties::VisualStyle style)
{
  return m_modeCtx->addSerie(id, title, style);
}

QFont PlotContextLimited::axisFont(const SerieProperties::Axis a) const
{
  return m_modeCtx->axisFont(a);
}

void PlotContextLimited::clearAllSerieSamples()
{
  m_modeCtx->clearAllSerieSamples();
}

void PlotContextLimited::clearSerieSamples(const int id)
{
  m_modeCtx->clearSerieSamples(id);
}

void PlotContextLimited::disableAutoscale()
{
  m_modeCtx->disableAutoscale();
}

void PlotContextLimited::enableAutoscale()
{
  m_modeCtx->enableAutoscale();
}

void PlotContextLimited::hideSerie(const int id)
{
  m_modeCtx->hideSerie(id);
}

void PlotContextLimited::onPointHovered(const QPointF &point, const QPoint &cursor)
{
  emit pointHovered(point, cursor);
}

void PlotContextLimited::onPointSelected(const QPointF &point, const QPoint &cursor)
{
  emit pointSelected(point, cursor);
}

void PlotContextLimited::removeSerie(const int id)
{
  m_modeCtx->removeSerie(id);
}

QRectF PlotContextLimited::range() const
{
  return m_modeCtx->range();
}

void PlotContextLimited::replot(const bool zoomOut)
{
  m_modeCtx->replot(zoomOut);
}

bool PlotContextLimited::serieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  return m_modeCtx->serieVisualStyle(id, style);
}

void PlotContextLimited::setAxisTitle(const SerieProperties::Axis axis, const QString &title)
{
  m_modeCtx->setAxisTitle(axis, title);
}

void PlotContextLimited::setAxisFont(const SerieProperties::Axis axis, const QFont &f)
{
  m_modeCtx->setAxisFont(axis, f);
}

void PlotContextLimited::setSerieSamples(const int id, const QVector<QPointF> &samples)
{
  m_modeCtx->setSerieSamples(id, samples);
}

void PlotContextLimited::setPlotTitle(const QString &title)
{
  m_modeCtx->setPlotTitle(title);
}

void PlotContextLimited::setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  m_modeCtx->setSerieVisualStyle(id, style);
}

void PlotContextLimited::showSerie(const int id)
{
  m_modeCtx->showSerie(id);
}



