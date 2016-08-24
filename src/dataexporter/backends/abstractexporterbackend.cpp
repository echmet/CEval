#include "abstractexporterbackend.h"

using namespace DataExporter;

AbstractExporterBackend::Cell::Cell(const QString &name, const QVariant &value, const uint32_t options) :
  name(name),
  value(value),
  options(options)
{
}

AbstractExporterBackend::AbstractExporterBackend(const Globals::DataArrangement arrangement) :
  m_arrangement(arrangement)
{
}

AbstractExporterBackend::~AbstractExporterBackend()
{
  releaseMatrix();
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
  int dimY = 0;
  int dimX = 0;

  for (const Block &b : m_blocks) {
    if (dimX < b.cells.size())
      dimX = b.cells.size();

    int _dimY = 1;
    for (const Cell *c : b.cells) {
      if (c == nullptr)
        continue;

      if (!(c->options & Cell::SINGLE))
        _dimY = 2;
    }
    dimY += _dimY;
  }

  if (m_arrangement == Globals::DataArrangement::VERTICAL)
    std::swap(dimX, dimY);

  OutputMatrix m = allocateOutputMatrix(dimX, dimY);

  int blockCtr = 0;
  for (const Block &b : m_blocks) {
    int next = 1;

    for (int jdx = 0; jdx < b.cells.size(); jdx++) {
      const Cell *c = b.cells.at(jdx);
      QString k;
      QString v;
      bool has_value = false;

      if (c != nullptr) {
        has_value = !(c->options & Cell::SINGLE);

        k = c->name;
        if (has_value)
          v = c->value.toString();
      }


      switch (m_arrangement) {
      case Globals::DataArrangement::HORIZONTAL:
        m[blockCtr][jdx] = k;
        if (has_value)
          m[blockCtr + 1][jdx] = v;
        break;
      case Globals::DataArrangement::VERTICAL:
        m[jdx][blockCtr] = k;
        if (has_value)
          m[jdx][blockCtr + 1] = v;
        break;
      }

      if (has_value)
        next = 2;
    }

    blockCtr += next;
  }

  return m;
}

void AbstractExporterBackend::releaseMatrix()
{
  for (Block &b : m_blocks) {
    for (const Cell *c : b.cells)
      delete c;
  }
}

void AbstractExporterBackend::reset()
{
  releaseMatrix();
  m_blocks.clear();
}
