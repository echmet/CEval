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
  int dimY = m_blocks.size();
  int dimX = 0;

  for (const Block &b : m_blocks) {
    if (b.cells.size() > dimX)
      dimX = b.cells.size();
  }

  dimY *= 2;

  OutputMatrix m = makeOutputMatrix(dimX, dimY);

  for (int idx = 0; idx < m_blocks.size(); idx++) {
    const Block &b = m_blocks.at(idx);
    OutputMatrixRow &rowKey = m[2*idx];
    OutputMatrixRow &rowValue = m[2*idx + 1];


    for (int jdx = 0; jdx < b.cells.size(); jdx++) {
      const Cell *c = b.cells.at(jdx);
      QString k;
      QString v;

      if (c != nullptr) {
        k = c->name;
        v = c->value.toString();
      }
      rowKey.push_back(k);
      rowValue.push_back(v);
    }
  }

  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QTextStream stream(&f);

  if (m_arrangement == Globals::DataArrangement::HORIZONTAL)
    return exportHorizontal(m, stream);
  else
    return exportVertical(m, stream, dimY, dimX);
}

bool TextExporterBackend::exportHorizontal(const OutputMatrix &m, QTextStream &stream)
{
  for (const OutputMatrixRow &row : m) {
    for (const QString &s : row)
      stream << s << m_delimiter;

    stream << "\n";
  }

  return true;
}

bool TextExporterBackend::exportVertical(const OutputMatrix &m, QTextStream &stream, const int rows, const int cols)
{
  for (int c = 0; c < cols; c++) {
    for (int r = 0; r < rows; r++)
      stream << m[r][c] << m_delimiter;

    stream << "\n";
  }

  return true;
}

TextExporterBackend::OutputMatrix TextExporterBackend::makeOutputMatrix(const int x, const int y)
{
  OutputMatrix m;

  m.reserve(y);

  for (int idx = 0; idx < y; idx++) {
    OutputMatrixRow row;

    row.reserve(x);
    m.push_back(row);
  }

  return m;
}
