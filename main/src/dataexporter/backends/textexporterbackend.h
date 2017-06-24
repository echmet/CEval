#ifndef TEXTEXPORTERBACKEND_H
#define TEXTEXPORTERBACKEND_H

#include "abstractexporterbackend.h"

class QTextStream;

namespace DataExporter {

class TextExporterBackend : public AbstractExporterBackend
{
public:
  explicit TextExporterBackend(const QString &path, const QChar &delimiter, const Globals::DataArrangement arrangement, const bool append = false);
  virtual bool exportData() override;

private:
  void write(const OutputMatrix &m, QTextStream &stream);

  const bool m_append;
  const QChar m_delimiter;
  const QString m_path;

};

} // namespace DataExporter

#endif // TEXTEXPORTERBACKEND_H
