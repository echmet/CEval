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
      SINGLE = 1        /*!< Cell is a single string and not a key-value pair */
    };

    explicit Cell(const QString &name, const QVariant &value, const uint32_t options = 0);

    const QString name;
    const QVariant value;
    const uint32_t options;

  };

  explicit AbstractExporterBackend(const Globals::DataArrangement arrangement);
  virtual ~AbstractExporterBackend();

  void addCell(Cell *cell, const int block, const int position);
  Globals::DataArrangement arrangement() const; /* UTILITARY HACK - there is no good reason to propagate this information!!! */
  virtual bool exportData() = 0;

protected:
  typedef QVector<QString> OutputMatrixRow;
  typedef QVector<QVector<QString>> OutputMatrix;

  class Block {
  public:
    QVector<Cell *> cells;
  };

  OutputMatrix allocateOutputMatrix(const int x, const int y);
  OutputMatrix makeOutputMatrix();
  void reset();

  Globals::DataArrangement m_arrangement;
  QVector<Block> m_blocks;

private:
  void releaseMatrix();

};

} // namespace DataExporter

#endif // ABSTRACTEXPORTERBACKEND_H
