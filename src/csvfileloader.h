#ifndef CSVFILELOADER_H
#define CSVFILELOADER_H

#include <QPointF>
#include <QMap>
#include <QVector>

class QTextStream;

class CsvFileLoader
{
public:
  class Encoding {
  public:
    explicit Encoding();
    Encoding(const QString &name, const QByteArray &bom, const QString &displayedName);
    Encoding(const Encoding &other);

    Encoding &operator=(const Encoding &other);

    const QString name;
    const QByteArray bom;
    const bool canHaveBom;
    const QString displayedName;
  };

  class Data {
  public:
    Data(const QVector<QPointF> &data, const QString &xType, const QString &yType);
    Data();
    bool isValid() const;
    Data &operator=(const Data &other);

    const QVector<QPointF> data;
    const QString xType;
    const QString yType;
  private:
    bool m_valid;

  };

  CsvFileLoader() = delete;

  static Data readClipboard(const QChar &delimiter, const QChar &decimalSeparator,
                            const int xColumn, const int yColumn,
                            const bool hasHeader, const quint32 linesToSkip,
                            const QString &encodingId);
  static Data readFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator,
                       const int xColumn, const int yColumn,
                       const bool hasHeader, const quint32 linesToSkip,
                       const QString &encodingId, const QByteArray &bom);

  static const QMap<QString, Encoding> SUPPORTED_ENCODINGS;

private:
  static Data readStream(QTextStream &stream, const QChar &delimiter, const QChar &decimalSeparator,
                         const int xColumn, const int yColumn,
                         const bool hasHeader, const quint32 linesToSkip);
  static void warnMalformedFile();

};

#endif // CSVFILELOADER_H
