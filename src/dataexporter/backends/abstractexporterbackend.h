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
    enum Options : uint32_t {
      SINGLE = 1 << 0,       /*!< Cell is a single string and not a key-value pair */
      CAPTION = 1 << 1
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
  enum OutputOptions : uint32_t {
    OO_CAPTION = 1 << 0
  };

  class Output {
  public:
    explicit Output();
    explicit Output(const QVariant &value, const uint32_t options = 0);
    Output & operator=(const Output &other);

    const QVariant value;
    const uint32_t options;
  };

  typedef QVector<Output> OutputMatrixRow;
  typedef QVector<OutputMatrixRow> OutputMatrix;

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
