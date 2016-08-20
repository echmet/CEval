#ifndef ABSTRACTEXPORTERBACKEND_H
#define ABSTRACTEXPORTERBACKEND_H

#include "../exporterglobals.h"
#include <QVariant>
#include <QVector>

namespace DataExporter {

class AbstractExporterBackend
{
public:
  class Cell {
  public:
    explicit Cell(const QString &name, const QVariant &value);

    const QString name;
    const QVariant value;
  };

  explicit AbstractExporterBackend(const Globals::DataArrangement arrangement);
  virtual ~AbstractExporterBackend();

  void addCell(Cell *cell, const int block, const int position);
  virtual bool exportData() = 0;

protected:
  class Block {
  public:
    QVector<Cell *> cells;
  };

  Globals::DataArrangement m_arrangement;
  QVector<Block> m_blocks;

};

} // namespace DataExporter

#endif // ABSTRACTEXPORTERBACKEND_H
