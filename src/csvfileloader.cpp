#include "csvfileloader.h"
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

bool CsvFileLoader::Data::isValid() const
{
  return m_valid;
}

CsvFileLoader::Data CsvFileLoader::loadFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const bool hasHeader,
                                            const QString &encodingId, const QByteArray &bom)
{
  QFile dataFile(path);
  QVector<QPointF> points;
  QString xType;
  QString yType;
  QTextStream stream;

  if (!dataFile.open(QIODevice::ReadOnly)) {
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

  if (hasHeader) {
    QStringList header;
    QString line;

    line = stream.readLine();
    header = line.split(delimiter.toLatin1());
    if (header.size() != 2) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"),
                           QString(QObject::tr("The selected file does not appear to have the \"time%1value\" format")).arg(delimiter));
      return Data();
    }
    xType = header.at(0);
    yType = header.at(1);
  }

  int lineCnt = hasHeader ? 2 : 1;
  const QChar qcDelimiter = delimiter.toLatin1();
  while (!dataFile.atEnd()) {
    QStringList values;
    QString line;
    qreal x, y;
    bool ok;
    QString *s;
    QLocale cLoc(QLocale::C);

    line = stream.readLine();
    values = line.split(qcDelimiter);
    if (values.size() != 2) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xType, yType);
    }

    s = &values[0];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    x = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"time\" on line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xType, yType);
    }

    s = &values[1];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Malformed line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    y = cLoc.toDouble(s, &ok);
    if (!ok) {
      QMessageBox::warning(nullptr, QObject::tr("Malformed file"), QString(QObject::tr("Invalid value for \"value\" on line %1. Data will be incomplete")).arg(lineCnt));
      return Data(points, xType, yType);
    }

    points.append(QPointF(x, y));
    lineCnt++;
  }

  return Data(points, xType, yType);
}

