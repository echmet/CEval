#include "csvfileloader.h"
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QTextStream>
#include "gui/malformedcsvfiledialog.h"

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

CsvFileLoader::Parameters::Parameters() :
  delimiter('\0'),
  decimalSeparator('.'),
  xColumn(0), yColumn(0),
  hasHeader(false),
  linesToSkip(0),
  encodingId(QString()),
  readBom(false),
  isValid(false)
{
}

CsvFileLoader::Parameters::Parameters(const QChar &delimiter, const QChar &decimalSeparator,
                                      const int xColumn, const int yColumn,
                                      const bool hasHeader, const int linesToSkip,
                                      const QString &encodingId, const bool &readBom) :
  delimiter(delimiter),
  decimalSeparator(decimalSeparator),
  xColumn(xColumn), yColumn(yColumn),
  hasHeader(hasHeader), linesToSkip(linesToSkip),
  encodingId(encodingId), readBom(readBom),
  isValid(true)
{
}

CsvFileLoader::Parameters & CsvFileLoader::Parameters::operator=(const Parameters &other)
{
  const_cast<QChar&>(delimiter) = other.delimiter;
  const_cast<QChar&>(decimalSeparator) = other.decimalSeparator;
  const_cast<int&>(xColumn) = other.xColumn;
  const_cast<int&>(yColumn) = other.yColumn;
  const_cast<bool&>(hasHeader) = other.hasHeader;
  const_cast<int&>(linesToSkip) = other.linesToSkip;
  const_cast<QString&>(encodingId) = other.encodingId;
  const_cast<bool&>(readBom) = other.readBom;
  const_cast<bool&>(isValid) = other.isValid;

  return *this;
}

void showMalformedFileError(const MalformedCsvFileDialog::Error err, const int lineNo, const QString &badLine)
{
  MalformedCsvFileDialog dlg(err, lineNo, badLine);
  dlg.exec();
}

CsvFileLoader::Data CsvFileLoader::readClipboard(const Parameters &params)
{
  QString clipboardText = QApplication::clipboard()->text();
  QTextStream stream;

  stream.setCodec(params.encodingId.toUtf8());
  stream.setString(&clipboardText);

  return readStream(stream, params.delimiter, params.decimalSeparator, params.xColumn, params.yColumn, params.hasHeader, params.linesToSkip);
}

CsvFileLoader::Data CsvFileLoader::readFile(const QString &path, const Parameters &params)
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
  if (params.readBom) {
    const QByteArray &bom = SUPPORTED_ENCODINGS[params.encodingId].bom;
    const QByteArray actualBom = dataFile.read(bom.size());

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
  stream.setCodec(params.encodingId.toUtf8());

  return readStream(stream, params.delimiter, params.decimalSeparator, params.xColumn, params.yColumn, params.hasHeader, params.linesToSkip);

}

CsvFileLoader::Data CsvFileLoader::readStream(QTextStream &stream, const QChar &delimiter, const QChar &decimalSeparator,
                                              const int xColumn, const int yColumn,
                                              const bool hasHeader, const int linesToSkip)
{
  QVector<QPointF> points;
  QString xType;
  QString yType;
  QStringList lines;
  int highColumn = (yColumn > xColumn) ? yColumn : xColumn;
  int linesRead = 0;
  int emptyLines = 0;

  /* Skip leading blank lines */
  while (!stream.atEnd()) {
    QString line = stream.readLine();

    if (line.trimmed() != QString("")) {
      lines.append(line);
      break;
    }

    emptyLines++;
  }

  /* Read the rest of the file */
  while (!stream.atEnd())
    lines.append(stream.readLine());

  if (lines.size() < 1) {
    QMessageBox::warning(nullptr, QObject::tr("No data"), QObject::tr("Input stream contains no data"));
    return Data();
  }

  if (lines.size() < linesToSkip + 1) {
    QMessageBox::warning(nullptr, QObject::tr("Invalid data"), QObject::tr("File contains less lines than the number of lines that were to be skipped"));
    return Data();
  }

  linesRead = linesToSkip;
  if (hasHeader) {
    QStringList header;
    const QString &line = lines.at(linesRead);

    header = line.split(delimiter);
    if (header.size() < highColumn) {
      showMalformedFileError(MalformedCsvFileDialog::Error::POSSIBLY_INCORRECT_SETTINGS, linesRead, line);

      return Data();
    }
    xType = header.at(xColumn - 1);
    yType = header.at(yColumn - 1);

    linesRead++;
  } else {
    /* Check file format and warn the user early if the expected format does not match to that of the file */
    const QString &line = lines.at(linesRead);
    const QStringList splitted = line.split(delimiter);

    if (splitted.size() < highColumn) {
      showMalformedFileError(MalformedCsvFileDialog::Error::POSSIBLY_INCORRECT_SETTINGS, linesRead, line);

      return Data();
    }
  }

  for (int idx = linesRead; idx < lines.size(); idx++) {
    QStringList values;
    qreal x, y;
    bool ok;
    QString *s;
    QLocale cLoc(QLocale::C);
    const QString &line = lines.at(idx);

    values = line.split(delimiter);
    if (values.size() < highColumn) {
      showMalformedFileError(MalformedCsvFileDialog::Error::BAD_DELIMITER, linesRead + emptyLines + 1, line);
      return Data(points, xType, yType);
    }

    s = &values[xColumn - 1];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      showMalformedFileError(MalformedCsvFileDialog::Error::BAD_DELIMITER, linesRead + emptyLines + 1, line);
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    x = cLoc.toDouble(s, &ok);
    if (!ok) {
      showMalformedFileError(MalformedCsvFileDialog::Error::BAD_TIME_DATA, linesRead + emptyLines + 1, line);
      return Data(points, xType, yType);
    }

    s = &values[yColumn - 1];
    /* Check that the string does not contain period as the default separator */
    if (decimalSeparator != '.' && s->contains('.')) {
      showMalformedFileError(MalformedCsvFileDialog::Error::BAD_DELIMITER, linesRead + emptyLines + 1, line);
      return Data(points, xType, yType);
    }

    s->replace(decimalSeparator, '.');
    y = cLoc.toDouble(s, &ok);
    if (!ok) {
      showMalformedFileError(MalformedCsvFileDialog::Error::BAD_VALUE_DATA, linesRead + emptyLines + 1, line);
      return Data(points, xType, yType);
    }

    points.append(QPointF(x, y));
    linesRead++;
  }

  return Data(points, xType, yType);
}
