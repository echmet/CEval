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
