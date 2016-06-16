#include "csvfileloader.h"
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QTextStream>

const QMap<QString, CsvFileLoader::Encoding> CsvFileLoader::SUPPORTED_ENCODINGS = { {"ISO-8859-1", CsvFileLoader::Encoding("ISO-8859-1", QByteArray(), "ISO-8859-1 (Latin 1)") },
                                                                                    {"ISO-8859-2", CsvFileLoader::Encoding("ISO-8859-2", QByteArray(), "ISO-8859-2 (Latin 2)") },
                                                                                    { "windows-1250", CsvFileLoader::Encoding("windows-1250", QByteArray(), "Windows-1250 (cp1250)") },
                                                                                    { "windows-1251", CsvFileLoader::Encoding("windows-1251", QByteArray(), "Windows-1251 (cp1251)") },
                                                                                    { "windows-1252", CsvFileLoader::Encoding("windows-1252", QByteArray(), "Windows-1252 (cp1252)") },
                                                                                    { "UTF-8", CsvFileLoader::Encoding("UTF-8", QByteArray("\xEF\xBB\xBF", 3), "UTF-8") },
                                                                                    { "UTF-16LE", CsvFileLoader::Encoding("UTF-16LE", QByteArray("\xFF\xFE", 2), "UTF-16LE (Little Endian)") },
                                                                                    { "UTF-16BE", CsvFileLoader::Encoding("UTF-16BE", QByteArray("\xFF\xFF", 2), "UTF-16BE (Big Endian)") },
                                                                                    { "UTF-32LE", CsvFileLoader::Encoding("UTF-32LE", QByteArray("\xFF\xFE\x00\x00", 4), "UTF-32LE (Little Endian)") },
                                                                                    { "UTF-32BE", CsvFileLoader::Encoding("UTF-32BE", QByteArray("\x00\x00\xFF\xFF", 4), "UTF-32BE (Big Endian)") } };

CsvFileLoader::Encoding::Encoding() :
  name(""),
  bom(QByteArray()),
  canHaveBom(false),
  displayedName("")
{
}

CsvFileLoader::Encoding::Encoding(const QString &name, const QByteArray &bom, const QString &displayedName) :
  name(name),
  bom(bom),
  canHaveBom(bom.size() > 0 ? true : false),
  displayedName(displayedName)
{
}

CsvFileLoader::Encoding::Encoding(const Encoding &other) :
  name(other.name),
  bom(other.bom),
  canHaveBom(other.canHaveBom),
  displayedName(other.displayedName)
{
}

CsvFileLoader::Encoding &CsvFileLoader::Encoding::operator=(const CsvFileLoader::Encoding &other)
{
  const_cast<QString&>(name) = other.name;
  const_cast<QByteArray&>(bom) = other.bom;
  const_cast<bool&>(canHaveBom) = other.canHaveBom;
  const_cast<QString&>(displayedName) = other.displayedName;

  return *this;
}

CsvFileLoader::Data::Data(const QVector<QPointF> &data, const QString &xType, const QString &yType) :
  data(data),
  xType(xType),
  yType(yType),
  m_valid(true)
{
}

CsvFileLoader::Data::Data() :
  data(QVector<QPointF>()),
  xType(""),
  yType(""),
  m_valid(false)
{
}

CsvFileLoader::Data &CsvFileLoader::Data::operator=(const Data &other)
{
  const_cast<QVector<QPointF>&>(data) = other.data;
  const_cast<QString&>(xType) = other.yType;
  const_cast<QString&>(yType) = other.yType;
  const_cast<bool&>(m_valid) = other.m_valid;

  return *this;
}

bool CsvFileLoader::Data::isValid() const
{
  return m_valid;
}

CsvFileLoader::Data CsvFileLoader::readClipboard(const QChar &delimiter, const QChar &decimalSeparator,
                                                 const int xColumn, const int yColumn,
                                                 const bool hasHeader, const quint32 linesToSkip,
                                                 const QString &encodingId)
{
  QString clipboardText = QApplication::clipboard()->text();
  QTextStream stream;

  stream.setCodec(encodingId.toUtf8());
  stream.setString(&clipboardText);

  return readStream(stream, delimiter, decimalSeparator, xColumn, yColumn, hasHeader, linesToSkip);
}

CsvFileLoader::Data CsvFileLoader::readFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator,
                                            const int xColumn, const int yColumn,
                                            const bool hasHeader, const quint32 linesToSkip,
                                            const QString &encodingId, const QByteArray &bom)
{
  QFile dataFile(path);
  QTextStream stream;

  if (!dataFile.exists()) {
    QMessageBox::warning(nullptr, QObject::tr("Invalid file"), QObject::tr("Specified file does not exist"));

    return Data();
  }

  if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QObject::tr("Cannot open file"), QString(QObject::tr("Cannot open the specified file for reading\n"
                                                                                       "Error reported: %1")).arg(dataFile.errorString()));
    return Data();
  }

  /* Check BOM */
  if (bom.size() > 0) {
    QByteArray actualBom = dataFile.read(bom.size());

    if (actualBom.size() != bom.size()) {
      QMessageBox::warning(nullptr, QObject::tr("Cannot read file"), QObject::tr("Byte order mark was expected but not found"));
      return Data();
    }

    if (memcmp(actualBom.data(), bom.data(), bom.size())) {
      QMessageBox::warning(nullptr, QObject::tr("Cannot read file"), QObject::tr("Byte order mark does not match to that expected for the given encoding"));
      return Data();
    }
  }

  stream.setDevice(&dataFile);
  stream.setCodec(encodingId.toUtf8());

  return readStream(stream, delimiter, decimalSeparator, xColumn, yColumn, hasHeader, linesToSkip);

}

CsvFileLoader::Data CsvFileLoader::readStream(QTextStream &stream, const QChar &delimiter, const QChar &decimalSeparator,
                                              const int xColumn, const int yColumn,
                                              const bool hasHeader, const quint32 linesToSkip)
{
  QVector<QPointF> points;
  QString xType;
  QString yType;
  quint32 linesRead = 0;
  int highColumn = (yColumn > xColumn) ? yColumn : xColumn;

  if (linesToSkip > 0) {
    while (linesRead < linesToSkip) {
      stream.readLine();
      linesRead++;
    }
  } else if (hasHeader) {
    QStringList header;
    QString line;

    line = stream.readLine();
    header = line.split(delimiter.toLatin1());
    if (header.size() < highColumn) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"),
                           QString(QObject::tr("The selected file does not appear to have the desired format")).arg(delimiter));
      return Data();
    }
    xType = header.at(xColumn - 1);
    yType = header.at(yColumn - 1);

    linesRead++;
  }

  const QChar qcDelimiter = delimiter;
  while (!stream.atEnd()) {
    QStringList values;
    QString line;
    qreal x, y;
    bool ok;
    QString *s;
    QLocale cLoc(QLocale::C);

    line = stream.readLine();
    values = line.split(qcDelimiter);
    if (values.size() < highColumn) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(linesRead + 1));
      return Data(points, xType, yType);
    }

    s = &values[xColumn - 1];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(linesRead + 1));
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    x = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"time\" on line %1. Data will be incomplete")).arg(linesRead + 1));
      return Data(points, xType, yType);
    }

    s = &values[yColumn - 1];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(linesRead + 1));
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    y = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"value\" on line %1. Data will be incomplete")).arg(linesRead + 1));
      return Data(points, xType, yType);
    }

    points.append(QPointF(x, y));
    linesRead++;
  }

  return Data(points, xType, yType);
}

