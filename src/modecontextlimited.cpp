#include "modecontextlimited.h"
#include "modecontext.h"

ModeContextLimited::ModeContextLimited(std::shared_ptr<ModeContext> modeCtx, QObject *parent) :
  QObject(parent),
  m_modeCtx(modeCtx)
{
  connect(m_modeCtx.get(), &ModeContext::pointSelected, this, &ModeContextLimited::onPointSelected);
}

bool ModeContextLimited::addSerie(const int id, const QString &title, SerieProperties::VisualStyle style)
{
  return m_modeCtx->addSerie(id, title, style);
}

QFont ModeContextLimited::axisFont(const SerieProperties::Axis a) const
{
  return m_modeCtx->axisFont(a);
}

void ModeContextLimited::clearAllSerieSamples()
{
  m_modeCtx->clearAllSerieSamples();
}

void ModeContextLimited::clearSerieSamples(const int id)
{
  m_modeCtx->clearSerieSamples(id);
}


void ModeContextLimited::hideSerie(const int id)
{
  m_modeCtx->hideSerie(id);
}

void ModeContextLimited::onPointSelected(const QPointF &point, const QPoint &cursor)
{
  emit pointSelected(point, cursor);
}

void ModeContextLimited::removeSerie(const int id)
{
  m_modeCtx->removeSerie(id);
}

QRectF ModeContextLimited::range() const
{
  return m_modeCtx->range();
}

void ModeContextLimited::replot()
{
  m_modeCtx->replot();
}

bool ModeContextLimited::serieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  return m_modeCtx->serieVisualStyle(id, style);
}

void ModeContextLimited::setAxisTitle(const SerieProperties::Axis axis, const QString &title)
{
  m_modeCtx->setAxisTitle(axis, title);
}

void ModeContextLimited::setAxisFont(const SerieProperties::Axis axis, const QFont &f)
{
  m_modeCtx->setAxisFont(axis, f);
}

void ModeContextLimited::setSerieSamples(const int id, const QVector<QPointF> &samples)
{
  m_modeCtx->setSerieSamples(id, samples);
}

void ModeContextLimited::setPlotTitle(const QString &title)
{
  m_modeCtx->setPlotTitle(title);
}

void ModeContextLimited::setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style)
{
  m_modeCtx->setSerieVisualStyle(id, style);
}

void ModeContextLimited::showSerie(const int id)
{
  m_modeCtx->showSerie(id);
}



