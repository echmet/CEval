#include "abstractexporterbackend.h"

using namespace DataExporter;

AbstractExporterBackend::Cell::Cell(const QString &name, const QVariant &value) :
  name(name),
  value(value)
{
}

AbstractExporterBackend::AbstractExporterBackend(const Globals::DataArrangement arrangement) :
  m_arrangement(arrangement)
{
}

AbstractExporterBackend::~AbstractExporterBackend()
{
  for (Block &b : m_blocks) {
    for (const Cell *c : b.cells)
      delete c;
  }
}

void AbstractExporterBackend::addCell(Cell *cell, const int block, const int position)
{
  if (m_blocks.size() <= block)
    m_blocks.resize(block + 1);

  Block &b = m_blocks[block];

  if (b.cells.size() <= position)
    b.cells.resize(position + 1);

  b.cells[position] = cell;
}

AbstractExporterBackend::OutputMatrix AbstractExporterBackend::allocateOutputMatrix(const int x, const int y)
{
  OutputMatrix m;

  m.reserve(y);

  for (int idx = 0; idx < y; idx++) {
    OutputMatrixRow row;

    row.resize(x);
    m.push_back(row);
  }

  return m;
}

AbstractExporterBackend::OutputMatrix AbstractExporterBackend::makeOutputMatrix()
{
  int dimY = m_blocks.size();
  int dimX = 0;

  for (const Block &b : m_blocks) {
    if (b.cells.size() > dimX)
      dimX = b.cells.size();
  }

  dimY *= 2;

  if (m_arrangement == Globals::DataArrangement::VERTICAL)
    std::swap(dimX, dimY);

  OutputMatrix m = allocateOutputMatrix(dimX, dimY);

  for (int idx = 0; idx < m_blocks.size(); idx++) {
    const Block &b = m_blocks.at(idx);

    for (int jdx = 0; jdx < b.cells.size(); jdx++) {
      const Cell *c = b.cells.at(jdx);
      QString k;
      QString v;

      if (c != nullptr) {
        k = c->name;
        v = c->value.toString();
      }

      switch (m_arrangement) {
      case Globals::DataArrangement::HORIZONTAL:
        m[2*idx][jdx] = k;
        m[2*idx + 1][jdx] = v;
        break;
      case Globals::DataArrangement::VERTICAL:
        m[jdx][2*idx] = k;
        m[jdx][2*idx + 1] = v;
        break;
      }
    }
  }

  return m;
}
