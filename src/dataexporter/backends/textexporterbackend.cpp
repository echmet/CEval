#include "textexporterbackend.h"
#include <QFile>
#include <QTextStream>

using namespace DataExporter;

TextExporterBackend::TextExporterBackend(const QString &path, const QChar &delimiter, const Globals::DataArrangement arrangement) :
  AbstractExporterBackend(arrangement),
  m_delimiter(delimiter),
  m_path(path)
{
}

bool TextExporterBackend::exportData()
{
  QFile f(m_path);

  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QTextStream stream(&f);

  OutputMatrix m = makeOutputMatrix();

  write(m, stream);

  return true;
}

void TextExporterBackend::write(const OutputMatrix &m, QTextStream &stream)
{
  for (const OutputMatrixRow &row : m) {
    for (const QString &s : row)
      stream << s << m_delimiter;

    stream << "\n";
  }
}



