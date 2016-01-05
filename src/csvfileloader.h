#ifndef CSVFILELOADER_H
#define CSVFILELOADER_H

#include <QPointF>
#include <QVector>

class CsvFileLoader
{
public:
  class Data {
  public:
    Data(const QVector<QPointF> &data, const QString &xUnit, const QString &yUnit);
    Data();
    bool isValid() const;

    const QVector<QPointF> data;
    const QString xUnit;
    const QString yUnit;
  private:
    bool m_valid;

  };

  CsvFileLoader() = delete;

  static Data loadFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const bool hasHeader);

};

#endif // CSVFILELOADER_H
