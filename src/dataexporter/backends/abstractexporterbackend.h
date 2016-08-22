#ifndef ABSTRACTEXPORTERBACKEND_H
#define ABSTRACTEXPORTERBACKEND_H

#include "../exporterglobals.h"
#include <QVariant>
#include <QVector>
#include <cstdint>

namespace DataExporter {

class AbstractExporterBackend
{
public:
  class Cell {
  public:
    enum Options {
      NO_VALUE = 1
    };

    explicit Cell(const QString &name, const QVariant &value, const uint32_t options = 0);

    const QString name;
    const QVariant value;
    const uint32_t options;

  };

  explicit AbstractExporterBackend(const Globals::DataArrangement arrangement);
  virtual ~AbstractExporterBackend();

  void addCell(Cell *cell, const int block, const int position);
  virtual bool exportData() = 0;
  void reset();

protected:
  typedef QVector<QString> OutputMatrixRow;
  typedef QVector<QVector<QString>> OutputMatrix;

  class Block {
  public:
    QVector<Cell *> cells;
  };

  OutputMatrix allocateOutputMatrix(const int x, const int y);
  OutputMatrix makeOutputMatrix();

  Globals::DataArrangement m_arrangement;
  QVector<Block> m_blocks;

private:
  void releaseMatrix();

};

} // namespace DataExporter

#endif // ABSTRACTEXPORTERBACKEND_H
