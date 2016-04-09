#include "plotexporter.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QImageWriter>
#include <QMessageBox>
#include <qwt_plot.h>
#include <qwt_plot_renderer.h>
#include <qwt_scale_widget.h>

#include <QDebug>

PlotExporter::PlotExporter(QObject *parent) : QObject(parent)
{
  for (const QByteArray &a : QImageWriter::supportedImageFormats())
    m_supportedFormats.push_back(QString(a));

  /* Add formats supported by QwtPlotRenderer */
  m_supportedFormats << "pdf" << "svg" << "ps";

  m_exportDlg = new ExportPlotToImageDialog(m_supportedFormats);

  m_plotPalette.setColor(QPalette::WindowText, Qt::black);
  m_plotPalette.setColor(QPalette::Foreground, Qt::black);
  m_plotPalette.setColor(QPalette::Text, Qt::black);
}

PlotExporter::~PlotExporter()
{
  delete m_exportDlg;
}

void PlotExporter::exportPlot(QwtPlot *plot)
{
  QSizeF guessedDimensions;
  ExportPlotToImageDialog::Parameters p = m_exportDlg->parameters();

  guessPlotDimensions(plot, p.dimensions.width(), guessedDimensions);

  m_exportDlg->setPlotDimensions(guessedDimensions);

  while (m_exportDlg->exec() == QDialog::Accepted) {
    p = m_exportDlg->parameters();

    QString path;

    if (p.path.length() < 0) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid path"));
      continue;
    }
    if (!m_supportedFormats.contains(p.format)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid output format"));
      continue;
    }

    if (p.path.endsWith("." + p.format))
      path = p.path;
    else
      path = p.path + "." + p.format;

    /* Scale up from millimeters to centimeters*/
    QSizeF dimensionsMM(p.dimensions.width() * 10.0, p.dimensions.height() * 10.0);

    /* Store current properties of the plot as we need to change them for rendering */
    QPalette storedPalette = plot->palette();
    QPalette xBottomPalette = plot->axisWidget(QwtPlot::xBottom)->palette();
    QPalette xTopPalette = plot->axisWidget(QwtPlot::xTop)->palette();
    QPalette yLeftPalette = plot->axisWidget(QwtPlot::yLeft)->palette();
    QPalette yRightPalette = plot->axisWidget(QwtPlot::yRight)->palette();
    QFont xBottomFont = plot->axisWidget(QwtPlot::xBottom)->font();
    QFont xTopFont = plot->axisWidget(QwtPlot::xTop)->font();
    QFont yLeftFont = plot->axisWidget(QwtPlot::yLeft)->font();
    QFont yRightFont = plot->axisWidget(QwtPlot::yRight)->font();
    QFont xBottomTitleFont = plot->axisTitle(QwtPlot::xBottom).font();
    QFont xTopTitleFont = plot->axisTitle(QwtPlot::xTop).font();
    QFont yLeftTitleFont = plot->axisTitle(QwtPlot::yLeft).font();
    QFont yRightTitleFont = plot->axisTitle(QwtPlot::yRight).font();
    QFont titleFont = plot->title().font();

    /* Recalculate font size by the DPI for every font */
    double outputInPixels = (static_cast<double>(p.dimensions.width()) / 2.54) * p.dpi;

    const double scalingRatio = (static_cast<double>(qApp->desktop()->logicalDpiX()) / p.dpi) * (outputInPixels / plot->geometry().width());
    QFont _xBottomFont(xBottomFont);
    QFont _xTopFont(xTopFont);
    QFont _yLeftFont(yLeftFont);
    QFont _yRightFont(yRightFont);
    QFont _xBottomTitleFont(xBottomTitleFont);
    QFont _xTopTitleFont(xTopTitleFont);
    QFont _yLeftTitleFont(yLeftTitleFont);
    QFont _yRightTitleFont(yRightTitleFont);
    QFont _titleFont;
    _xBottomFont.setPointSizeF(xBottomFont.pointSizeF() * scalingRatio);
    _xTopFont.setPointSizeF(xTopFont.pointSizeF() * scalingRatio);
    _yLeftFont.setPointSizeF(yLeftFont.pointSizeF() * scalingRatio);
    _yRightFont.setPointSizeF(yRightFont.pointSizeF() * scalingRatio);
    _xBottomTitleFont.setPointSizeF(xBottomTitleFont.pointSizeF() * scalingRatio);
    _xTopTitleFont.setPointSizeF(xTopTitleFont.pointSizeF() * scalingRatio);
    _yLeftTitleFont.setPointSizeF(yLeftTitleFont.pointSizeF() * scalingRatio);
    _yRightTitleFont.setPointSizeF(yRightTitleFont.pointSizeF() * scalingRatio);
    _titleFont.setPointSizeF(titleFont.pointSizeF() * scalingRatio);

    plot->setPalette(m_plotPalette);
    plot->axisWidget(QwtPlot::xBottom)->setPalette(m_plotPalette);
    plot->axisWidget(QwtPlot::xTop)->setPalette(m_plotPalette);
    plot->axisWidget(QwtPlot::yLeft)->setPalette(m_plotPalette);
    plot->axisWidget(QwtPlot::yRight)->setPalette(m_plotPalette);
    plot->axisWidget(QwtPlot::xBottom)->setFont(_xBottomFont);
    plot->axisWidget(QwtPlot::xTop)->setFont(_xTopFont);
    plot->axisWidget(QwtPlot::yLeft)->setFont(_yLeftFont);
    plot->axisWidget(QwtPlot::yRight)->setFont(_yRightFont);
    setAxisTitleFont(plot, QwtPlot::xBottom, _xBottomTitleFont);
    setAxisTitleFont(plot, QwtPlot::xTop, _xTopTitleFont);
    setAxisTitleFont(plot, QwtPlot::yLeft, _yLeftTitleFont);
    setAxisTitleFont(plot, QwtPlot::yRight, _yRightTitleFont);
    setTitleFont(plot, _titleFont);

    renderPlotToFile(plot, path, p.format, dimensionsMM, p.dpi);

    /* Restore the origina plot properties */
    plot->setPalette(storedPalette);
    plot->axisWidget(QwtPlot::xBottom)->setPalette(xBottomPalette);
    plot->axisWidget(QwtPlot::xTop)->setPalette(xTopPalette);
    plot->axisWidget(QwtPlot::yLeft)->setPalette(yLeftPalette);
    plot->axisWidget(QwtPlot::yRight)->setPalette(yRightPalette);
    plot->axisWidget(QwtPlot::xBottom)->setFont(xBottomFont);
    plot->axisWidget(QwtPlot::xTop)->setFont(xTopFont);
    plot->axisWidget(QwtPlot::yLeft)->setFont(yLeftFont);
    plot->axisWidget(QwtPlot::yRight)->setFont(yRightFont);
    setAxisTitleFont(plot, QwtPlot::xBottom, xBottomTitleFont);
    setAxisTitleFont(plot, QwtPlot::xTop, xTopTitleFont);
    setAxisTitleFont(plot, QwtPlot::yLeft, yLeftTitleFont);
    setAxisTitleFont(plot, QwtPlot::yRight, yRightTitleFont);
    setTitleFont(plot, titleFont);
    break;
  }
}

void PlotExporter::guessPlotDimensions(const QwtPlot *plot, const double currentWidth, QSizeF &dimensions)
{
  const QRect &plotDims = plot->geometry();
  const double plotRatio = static_cast<double>(plotDims.width()) / static_cast<double>(plotDims.height());

  dimensions.setWidth(currentWidth);
  dimensions.setHeight(currentWidth / plotRatio);
}

void PlotExporter::renderPlotToFile(QwtPlot *plot, const QString &path, const QString &format,
                                    const QSizeF &dimensions, const int dpi)
{
  QwtPlotRenderer renderer(this);

  renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
  renderer.setDiscardFlag(QwtPlotRenderer::DiscardFooter, true);
  renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame, true);
  renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales, true);

  renderer.renderDocument(plot, path, format, dimensions, dpi);
}


void PlotExporter::setAxisTitleFont(QwtPlot *plot, const int a, const QFont &f)
{
  QwtText t = plot->axisTitle(a);
  t.setFont(f);
  plot->setAxisTitle(a, t);
}

void PlotExporter::setTitleFont(QwtPlot *plot, const QFont &f)
{
  QwtText t = plot->title();
  t.setFont(f);
  plot->setTitle(t);
}
