#include "textstreamexporterbackend.h"
#include <QTextStream>

using namespace DataExporter;

TextStreamExporterBackend::TextStreamExporterBackend(QTextStream *outStream, const QChar &delimiter, const Globals::DataArrangement arrangement) :
  AbstractExporterBackend(arrangement),
  m_delimiter(delimiter),
  m_outStream(outStream)
{
}

void TextStreamExporterBackend::clear()
{
  reset();
}

bool TextStreamExporterBackend::exportData()
{
  OutputMatrix m = makeOutputMatrix();

  for (const OutputMatrixRow &row : m) {
    for (const Output &o : row)
      *m_outStream << o.value.toString() << m_delimiter;

    *m_outStream << "\n";
  }

  return true;
}
