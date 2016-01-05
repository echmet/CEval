#include "csvfileloader.h"
#include <QFile>
#include <QLocale>
#include <QMessageBox>

CsvFileLoader::Data::Data(const QVector<QPointF> &data, const QString &xUnit, const QString &yUnit) :
  data(data),
  xUnit(xUnit),
  yUnit(yUnit),
  m_valid(true)
{
}

CsvFileLoader::Data::Data() :
  data(QVector<QPointF>()),
  xUnit(""),
  yUnit(""),
  m_valid(false)
{
}

bool CsvFileLoader::Data::isValid() const
{
  return m_valid;
}

CsvFileLoader::Data CsvFileLoader::loadFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const bool hasHeader)
{
  QFile dataFile(path);
  QVector<QPointF> points;
  QString xUnit;
  QString yUnit;

  if (!dataFile.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(nullptr, QObject::tr("Cannot open file"), QString(QObject::tr("Cannot open the specified file for reading\n"
                                                                                       "Error reported: %1")).arg(dataFile.errorString()));
    return Data();
  }

  if (hasHeader) {
    QList<QByteArray> header;
    QByteArray line;

    line = dataFile.readLine();
    header = line.split(delimiter.toLatin1());
    if (header.size() != 2) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"),
                           QString(QObject::tr("The selected file does not appear to have the \"time%1value\" format")).arg(delimiter));
      return Data();
    }
    xUnit = QString(header.at(0));
    yUnit = QString(header.at(1));
  }

  int lineCnt = hasHeader ? 2 : 1;
  while (!dataFile.atEnd()) {
    QList<QByteArray> values;
    QByteArray line;
    qreal x, y;
    bool ok;
    QString s;
    QLocale cLoc(QLocale::C);

    line = dataFile.readLine();
    values = line.split(delimiter.toLatin1());
    if (values.size() != 2) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xUnit, yUnit);
    }

    s = values.at(0);
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s.contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xUnit, yUnit);
    }

    s.replace(decimalSeparator, '.');
    x = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"time\" on line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xUnit, yUnit);
    }

    s = QString(values.at(1));
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s.contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xUnit, yUnit);
    }

    s.replace(decimalSeparator, '.');
    y = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"value\" on line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xUnit, yUnit);
    }

    points.append(QPointF(x, y));
    lineCnt++;
  }

  return Data(points, xUnit, yUnit);
}

