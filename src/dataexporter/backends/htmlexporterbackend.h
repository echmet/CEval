#ifndef HTMLEXPORTERBACKEND_H
#define HTMLEXPORTERBACKEND_H

#include "abstractexporterbackend.h"

class QTextStream;

namespace DataExporter {

class HtmlExporterBackend : public AbstractExporterBackend
{
public:
  explicit HtmlExporterBackend(const QString &path, const Globals::DataArrangement arrangement);
  virtual bool exportData() override;

private:
  void write(const OutputMatrix &m, QTextStream &stream);

  const QString m_path;

};

} // namespace DataExporter

#endif // HTMLEXPORTERBACKEND_H
