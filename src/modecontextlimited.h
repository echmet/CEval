#ifndef MODECONTEXTLIMITED_H
#define MODECONTEXTLIMITED_H

#include <memory>
#include <QPointF>
#include <QVector>
#include "serieproperties.h"

class ModeContext;

class ModeContextLimited : public QObject
{
  Q_OBJECT
public:
  explicit ModeContextLimited(std::shared_ptr<ModeContext> modeCtx, QObject *parent = nullptr);
  bool addSerie(const int id, const QString &title, SerieProperties::VisualStyle style = SerieProperties::VisualStyle());
  QFont axisFont(const SerieProperties::Axis a) const;
  void clearAllSerieSamples();
  void clearSerieSamples(const int id);
  void hideSerie(const int id);
  void removeSerie(const int id);
  QRectF range() const;
  void replot(const bool zoomOut = true);
  bool serieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void setAxisTitle(const SerieProperties::Axis axis, const QString &title);
  void setAxisFont(const SerieProperties::Axis axis, const QFont &f);
  void setSerieSamples(const int id, const QVector<QPointF> &samples);
  void setPlotTitle(const QString &title);
  void setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void showSerie(const int id);

private:
  std::shared_ptr<ModeContext> m_modeCtx;

signals:
  void pointHovered(const QPointF &point, const QPoint &cursor);
  void pointSelected(const QPointF &point, const QPoint &cursor);

private slots:
  void onPointHovered(const QPointF &point, const QPoint &cursor);
  void onPointSelected(const QPointF &point, const QPoint &cursor);

};

#endif // MODECONTEXTLIMITED_H
