#include "plotcontextlimited.h"
#include "plotcontext.h"

PlotContextLimited::PlotContextLimited(std::shared_ptr<PlotContext> modeCtx, QObject *parent) :
  QObject(parent),
  m_plotCtx(modeCtx)
{
  connect(m_plotCtx.get(), &PlotContext::pointHovered, this, &PlotContextLimited::onPointHovered);
  connect(m_plotCtx.get(), &PlotContext::pointSelected, this, &PlotContextLimited::onPointSelected);
}

bool PlotContextLimited::addSerie(const int id, const QString &title, SerieProperties::VisualStyle style)
{
  return m_plotCtx->addSerie(id, title, style);
}

QFont PlotContextLimited::axisFont(const SerieProperties::Axis a) const
{
  return m_plotCtx->axisFont(a);
}

void PlotContextLimited::clearAllSerieSamples()
{
  m_plotCtx->clearAllSerieSamples();
}

void PlotContextLimited::clearSerieSamples(const int id)
{
  m_plotCtx->clearSerieSamples(id);
}

void PlotContextLimited::hideSerie(const int id)
{
  m_plotCtx->hideSerie(id);
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
  m_plotCtx->removeSerie(id);
}

QRectF PlotContextLimited::range() const
{
  return m_plotCtx->range();
}

void PlotContextLimited::replot()
{
  m_plotCtx->replot();
}

void PlotContextLimited::scaleToFit()
{
  m_plotCtx->scaleToFit();
}

bool PlotContextLimited::serieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  return m_plotCtx->serieVisualStyle(id, style);
}

void PlotContextLimited::setAxisTitle(const SerieProperties::Axis axis, const QString &title)
{
  m_plotCtx->setAxisTitle(axis, title);
}

void PlotContextLimited::setAxisFont(const SerieProperties::Axis axis, const QFont &f)
{
  m_plotCtx->setAxisFont(axis, f);
}

void PlotContextLimited::setSerieSamples(const int id, const QVector<QPointF> &samples)
{
  m_plotCtx->setSerieSamples(id, samples);
}

void PlotContextLimited::setPlotTitle(const QString &title)
{
  m_plotCtx->setPlotTitle(title);
}

void PlotContextLimited::setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  m_plotCtx->setSerieVisualStyle(id, style);
}

void PlotContextLimited::showSerie(const int id)
{
  m_plotCtx->showSerie(id);
}



