#ifndef TEXTEXPORTERBACKEND_H
#define TEXTEXPORTERBACKEND_H

#include "abstractexporterbackend.h"

class QTextStream;

namespace DataExporter {

class TextExporterBackend : public AbstractExporterBackend
{
public:
  explicit TextExporterBackend(const QString &path, const QChar &delimiter, const Globals::DataArrangement arrangement);
  virtual bool exportData() override;

private:
  typedef QVector<QString> OutputMatrixRow;
  typedef QVector<QVector<QString>> OutputMatrix;

  bool exportHorizontal(const OutputMatrix &m, QTextStream &stream);
  bool exportVertical(const OutputMatrix &m, QTextStream &stream, const int rows, const int cols);
  OutputMatrix makeOutputMatrix(const int x, const int y);

  const QChar m_delimiter;
  const QString m_path;

};

} // namespace DataExporter

#endif // TEXTEXPORTERBACKEND_H
