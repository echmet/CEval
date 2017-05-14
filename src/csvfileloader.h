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

  class Parameters {
  public:
    Parameters();
    Parameters(const QChar &delimiter, const QChar &decimalSeparator,
               const int xColumn, const int yColumn,
               const bool hasHeader, const int linesToSkip,
               const QString &encodingId, const bool &readBom);
    Parameters & operator=(const Parameters &other);

    const QChar delimiter;
    const QChar decimalSeparator;
    const int xColumn;
    const int yColumn;
    const bool hasHeader;
    const int linesToSkip;
    const QString encodingId;
    const bool readBom;
    const bool isValid;

  };

  CsvFileLoader() = delete;

  static Data readClipboard(const Parameters &params);
  static Data readFile(const QString &path, const Parameters &params);

  static const QMap<QString, Encoding> SUPPORTED_ENCODINGS;

private:
  static Data readStream(QTextStream &stream, const QChar &delimiter, const QChar &decimalSeparator,
                         const int xColumn, const int yColumn,
                         const bool hasHeader, const int linesToSkip);

};

#endif // CSVFILELOADER_H
