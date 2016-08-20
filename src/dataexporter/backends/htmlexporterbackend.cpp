#include "htmlexporterbackend.h"
#include <QFile>
#include <QTextStream>

using namespace DataExporter;

HtmlExporterBackend::HtmlExporterBackend(const QString &path, const Globals::DataArrangement arrangement) :
  AbstractExporterBackend(arrangement),
  m_path(path)
{
}

bool HtmlExporterBackend::exportData()
{
  QFile f(m_path);

  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QTextStream stream(&f);

  OutputMatrix m = makeOutputMatrix();

  write(m, stream);

  return true;
}

void HtmlExporterBackend::write(const OutputMatrix &m, QTextStream &stream)
{
  stream << "<html>\n\t<head>\n\t\t<title></title>\n\t</head>\n\n<body>\n\n<table>";

  for (const OutputMatrixRow &row : m) {
    stream << "<tr>";
    for (const QString &s : row)
      stream << "<td>" << s << "</td>";

    stream << "</tr>\n";
  }

  stream << "</table>\n\n</body>\n</html>\n";
}
