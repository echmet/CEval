#include "plotexporter.h"
#include <QFile>
#include <QImageWriter>
#include <QMessageBox>

PlotExporter::PlotExporter(QObject *parent) : QObject(parent)
{
  for (const QByteArray &a : QImageWriter::supportedImageFormats())
    m_supportedFormats.push_back(QString(a));

  m_exportDlg = new ExportPlotToImageDialog(m_supportedFormats);
}

PlotExporter::~PlotExporter()
{
  delete m_exportDlg;
}

void PlotExporter::exportToBitmap(QWidget *source)
{
  while (m_exportDlg->exec() == QDialog::Accepted) {
    ExportPlotToImageDialog::Parameters p = m_exportDlg->parameters();

    if (p.path.length() < 0) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid path"));
      continue;
    }
    if (!m_supportedFormats.contains(p.format)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid output format"));
      continue;
    }

    QPixmap pixmap(source->grab());
    writePixmapToFile(pixmap, p.path, p.format);
    break;
  }
}

void PlotExporter::writePixmapToFile(const QPixmap &pixmap, const QString &path, const QString &format)
{
  if (!pixmap.save(path, format.toUtf8().data(), 100)) {
    QMessageBox::warning(nullptr, QObject::tr("Unable to save pixmap"), QObject::tr("Unable to save plot to file"));
    return;
  }

}
