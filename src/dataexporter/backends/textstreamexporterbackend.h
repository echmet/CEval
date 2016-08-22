#ifndef TEXTSTREAMEXPORTERBACKEND_H
#define TEXTSTREAMEXPORTERBACKEND_H

#include "abstractexporterbackend.h"

class QTextStream;

namespace DataExporter {

class TextStreamExporterBackend : public AbstractExporterBackend
{
public:
  explicit TextStreamExporterBackend(QTextStream *outStream, const QChar &delimiter, const Globals::DataArrangement arrangement);
  virtual bool exportData() override;

private:
  QChar m_delimiter;
  QTextStream *m_outStream;

};

}

#endif // TEXTSTREAMEXPORTERBACKEND_H
