#ifndef PLOTCONTEXTLIMITED_H
#define PLOTCONTEXTLIMITED_H

#include <memory>
#include <QObject>
#include <QPointF>
#include <QVector>
#include "serieproperties.h"

class PlotContext;

class PlotContextLimited : public QObject
{
  Q_OBJECT
public:
  explicit PlotContextLimited(std::shared_ptr<PlotContext> modeCtx, QObject *parent = nullptr);
  bool addSerie(const int id, const QString &title, SerieProperties::VisualStyle style = SerieProperties::VisualStyle());
  QFont axisFont(const SerieProperties::Axis a) const;
  void clearAllSerieSamples();
  void clearSerieSamples(const int id);
  void hideSerie(const int id);
  void removeSerie(const int id);
  QRectF range() const;
  void replot();
  void scaleToFit();
  bool serieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void setAxisTitle(const SerieProperties::Axis axis, const QString &title);
  void setAxisFont(const SerieProperties::Axis axis, const QFont &f);
  void setSerieSamples(const int id, const QVector<QPointF> &samples);
  void setPlotTitle(const QString &title);
  void setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void showSerie(const int id);

  static const int DEFAULT_CENTRAL_POINT_SIZE;
  static const QwtSymbol::Style DEFAULT_CENTRAL_POINT_TYPE;
  static const int DEFAULT_POINT_SIZE;
  static const QwtSymbol::Style DEFAULT_POINT_TYPE;
  static const double DEFAULT_SERIES_WIDTH;

private:
  std::shared_ptr<PlotContext> m_plotCtx;

signals:
  void pointHovered(const QPointF &point, const QPoint &cursor);
  void pointSelected(const QPointF &point, const QPoint &cursor);

private slots:
  void onPointHovered(const QPointF &point, const QPoint &cursor);
  void onPointSelected(const QPointF &point, const QPoint &cursor);

};

#endif // PLOTCONTEXTLIMITED_H
