#include "textexporterbackend.h"
#include <QFile>
#include <QTextStream>

using namespace DataExporter;

TextExporterBackend::TextExporterBackend(const QString &path, const QChar &delimiter, const Globals::DataArrangement arrangement, const bool append) :
  AbstractExporterBackend(arrangement),
  m_append(append),
  m_delimiter(delimiter),
  m_path(path)
{
}

bool TextExporterBackend::exportData()
{
  QFile f(m_path);

  QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
  if (m_append)
    mode |= QIODevice::Append;

  if (!f.open(mode))
    return false;

  QTextStream stream(&f);

  OutputMatrix m = makeOutputMatrix();

  write(m, stream);

  return true;
}

void TextExporterBackend::write(const OutputMatrix &m, QTextStream &stream)
{
  for (const OutputMatrixRow &row : m) {
    for (const Output &o : row)
      stream << o.value.toString() << m_delimiter;

    stream << "\n";
  }
}



