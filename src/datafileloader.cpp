#include "datafileloader.h"
#include "custommetatypes.h"
#include "gui/loadchemstationdatadialog.h"
#include "gui/loadcsvfiledialog.h"
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

const QString DataFileLoader::LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG("LastChemStationLoadPath");
const QString DataFileLoader::LAST_CSV_LOAD_PATH_SETTINGS_TAG("LastCsvLoadPath");
const QString DataFileLoader::LAST_CHEMSTATION_DLG_SIZE_TAG("LastChemStationDlgSize");

DataFileLoader::Data::Data(const QVector<QPointF> data, const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit) :
  data(data),
  xType(xType),
  xUnit(xUnit),
  yType(yType),
  yUnit(yUnit),
  m_valid(true)
{
}

DataFileLoader::Data::Data() :
  m_valid(false)
{
}

DataFileLoader::Data::Data(const Data &other) :
  data(other.data),
  xType(other.xType),
  xUnit(other.xUnit),
  yType(other.yType),
  yUnit(other.yUnit),
  m_valid(other.m_valid)
{
}

DataFileLoader::Data &DataFileLoader::Data::operator=(const Data &other)
{
  const_cast<QVector<QPointF>&>(data) = other.data;
  const_cast<QString&>(xType) = other.xType;
  const_cast<QString&>(xUnit) = other.xUnit;
  const_cast<QString&>(yType) = other.yType;
  const_cast<QString&>(yUnit) = other.xUnit;
  const_cast<bool&>(m_valid) = other.m_valid;

  return *this;
}

DataFileLoader::DataFileLoader(QObject *parent) :
  QObject(parent),
  m_lastCsvPath(QDir::homePath()),
  m_lastChemStationDlgSize(QSize(0,0)),
  m_defaultPathsToTry([]() {
    QStringList pathsToTry = { QDir::homePath() };

    #ifdef Q_OS_UNIX
      pathsToTry.append( { "/home", "/" } );
    #elif defined Q_OS_WIN
      char drive = 'C';

      do {
        pathsToTry.append(QString(drive) + ":\\");
        drive++;
      } while (drive <= 'Z');

      pathsToTry.append( { "A:\\", "B:\\" } );
    #endif // Q_OS_

      return pathsToTry;
  }())
{
  m_lastChemStationPath = defaultPath();
  m_loadChemStationDataDlg = new LoadChemStationDataDialog();
  m_loadCsvFileDlg = new LoadCsvFileDialog();

  if (m_loadChemStationDataDlg == nullptr ||
      m_loadCsvFileDlg == nullptr)
    throw std::bad_alloc();

  for (const CsvFileLoader::Encoding &enc : CsvFileLoader::SUPPORTED_ENCODINGS)
    m_loadCsvFileDlg->addEncoding(enc.name, enc.displayedName, enc.canHaveBom);
}

DataFileLoader::~DataFileLoader()
{
  delete m_loadChemStationDataDlg;
  delete m_loadCsvFileDlg;
}

QString DataFileLoader::chemStationTypeToString(const ChemStationFileLoader::Type type)
{
  switch (type) {
  case ChemStationFileLoader::Type::CE_ANALOG:
    return "Analog input";
    break;
  case ChemStationFileLoader::Type::CE_CCD:
    return "Conductivity";
    break;
  case ChemStationFileLoader::Type::CE_CURRENT:
    return "Current";
    break;
  case ChemStationFileLoader::Type::CE_DAD:
    return "Absorbance";
    break;
  case ChemStationFileLoader::Type::CE_POWER:
    return "Power";
    break;
  case ChemStationFileLoader::Type::CE_PRESSURE:
    return "Pressure";
    break;
  case ChemStationFileLoader::Type::CE_TEMPERATURE:
    return "Temperature";
    break;
  case ChemStationFileLoader::Type::CE_VOLTAGE:
    return "Voltage";
    break;
  case ChemStationFileLoader::Type::CE_UNKNOWN:
  default:
    return "Unknown";
    break;
  }
}

QString DataFileLoader::defaultPath() const
{
  for (const QString &path : m_defaultPathsToTry) {
    QDir dir(path);

    if (dir.exists() && dir.isReadable())
      return path;
  }

  QMessageBox::critical(nullptr, tr("Runtime error"), tr("Unable to determine default path. The application may misbehave or crash."));

  return "";
}

bool DataFileLoader::isDirectoryUsable(const QString &path) const
{
  const QDir dir(path);

  return (dir.exists() && dir.isReadable());
}

void DataFileLoader::loadChemStationFile()
{
  int ret;

  if (m_lastChemStationDlgSize.width() > 0 && m_lastChemStationDlgSize.height() > 0)
      m_loadChemStationDataDlg->resize(m_lastChemStationDlgSize);

  if (!isDirectoryUsable(m_lastChemStationPath))
    m_lastChemStationPath = defaultPath();

  m_loadChemStationDataDlg->expandToPath(m_lastChemStationPath);

  ret = m_loadChemStationDataDlg->exec();
  m_lastChemStationDlgSize = m_loadChemStationDataDlg->size();

  if (ret != QDialog::Accepted)
    return;

  LoadChemStationDataDialog::LoadInfo loadInfo = m_loadChemStationDataDlg->loadInfo();

  switch (loadInfo.loadingMode) {
  case LoadChemStationDataDialog::LoadingMode::SINGLE_FILE:
    loadChemStationFileSingle(loadInfo.path);
    break;
  case LoadChemStationDataDialog::LoadingMode::MULTIPLE_DIRECTORIES:
    loadChemStationFileMultipleDirectories(loadInfo.dirPaths, loadInfo.filter);
    break;
  case LoadChemStationDataDialog::LoadingMode::WHOLE_DIRECTORY:
    loadChemStationFileWholeDirectory(loadInfo.path, loadInfo.filter);
    break;
  }
}

void DataFileLoader::loadChemStationFileSingle(const QString &path)
{
  ChemStationFileLoader::Data chData = ChemStationFileLoader::loadFile(path, true);

  if (!chData.isValid())
    return;

  QDir d(path);
  d.cdUp();
  m_lastChemStationPath = d.path();

  emit dataLoaded(std::shared_ptr<Data>(new Data(chData.data, "Time", "minute", chemStationTypeToString(chData.type), chData.yUnits)),
                  path, QFileInfo(path).fileName());
}

void DataFileLoader::loadChemStationFileMultipleDirectories(const QStringList &dirPaths, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(dirPaths, filter);

  for (const QString &path : files)
    loadChemStationFileSingle(path);
}

void DataFileLoader::loadChemStationFileWholeDirectory(const QString &path, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(path, filter);

  for (const QString &filePath : files)
    loadChemStationFileSingle(filePath);
}

void DataFileLoader::loadCsvFile(const bool readFromClipboard)
{
  LoadCsvFileDialog::Parameters p;
  QStringList filePaths;
  QFileDialog openDlg(nullptr, tr("Pick a comma-separated values file"), m_lastCsvPath);

  if (!readFromClipboard) {
    openDlg.setAcceptMode(QFileDialog::AcceptOpen);
    openDlg.setFileMode(QFileDialog::ExistingFiles);

    if (openDlg.exec() != QDialog::Accepted)
      return;

    filePaths = openDlg.selectedFiles();
    if (filePaths.length() < 1)
      return;
  }

  while (true) {
    if (m_loadCsvFileDlg->exec() != QDialog::Accepted)
      return;

    p = m_loadCsvFileDlg->parameters();
    if (p.delimiter.length() != 1 && (p.delimiter.compare("\\t") != 0)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter must be a single character."));
      continue;
    }
    if (p.decimalSeparator == p.delimiter) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter and decimal separator cannot be the same character."));
      continue;
    }

    if (p.xColumn == p.yColumn) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("X and Y columns cannot be the same"));
      continue;
    }

    break;
  }

  QChar delimiter;
  if (p.delimiter.compare("\\t") == 0)
    delimiter = '\t';
  else
    delimiter = p.delimiter.at(0);

  const QByteArray &bom = p.readBom == true ? CsvFileLoader::SUPPORTED_ENCODINGS[p.encodingId].bom : QByteArray();

  auto doLoad = [this, &p](const CsvFileLoader::Data &csvData, const QString &path) {
    QString xType;
    QString yType;
    QString xUnit;
    QString yUnit;
    QString fileName = "";

    switch (p.header) {
    case LoadCsvFileDialog::HeaderHandling::NO_HEADER:
      xType = p.xType;
      yType = p.yType;
      xUnit = p.xUnit;
      yUnit = p.yUnit;
      break;
    case LoadCsvFileDialog::HeaderHandling::HEADER_WITH_UNITS:
      xType = csvData.xType;
      yType = csvData.yType;
      break;
    case LoadCsvFileDialog::HeaderHandling::HEADER_WITHOUT_UNITS:
      xType = csvData.xType;
      yType = csvData.yType;
      xUnit = p.xUnit;
      yUnit = p.yUnit;
      break;
    }

    if (yUnit.length() > 0) {
      if (yUnit.at(yUnit.length() - 1) == '\n')
        yUnit.chop(1);
    }

    std::shared_ptr<Data> data = std::shared_ptr<Data>(new Data(csvData.data,
                                                       xType, xUnit,
                                                       yType, yUnit));

    if (path != "")
      fileName = QFileInfo(path).fileName();
    emit dataLoaded(data, path, fileName);
  };

  if (readFromClipboard) {
    CsvFileLoader::Data csvData = CsvFileLoader::readClipboard(delimiter, p.decimalSeparator, p.xColumn, p.yColumn,
                                                               p.header != LoadCsvFileDialog::HeaderHandling::NO_HEADER,
                                                               p.linesToSkip, p.encodingId);

    if (!csvData.isValid())
      return;

    doLoad(csvData, "");

  } else {
    for (const QString &path : filePaths) {
      CsvFileLoader::Data csvData = CsvFileLoader::readFile(path, delimiter, p.decimalSeparator,
                                                            p.xColumn, p.yColumn,
                                                            p.header != LoadCsvFileDialog::HeaderHandling::NO_HEADER, p.linesToSkip,
                                                            p.encodingId, bom);

      if (!csvData.isValid())
        continue;

      m_lastCsvPath = path;
      doLoad(csvData, path);
    }
  }

}

void DataFileLoader::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  EMT::StringVariantMap map = settings.value<EMT::StringVariantMap>();

  if (map.contains(LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG)) {
    const QVariant &v = map[LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG];

    m_lastChemStationPath = v.toString();
  }

  if (map.contains(LAST_CSV_LOAD_PATH_SETTINGS_TAG)) {
    const QVariant &v = map[LAST_CSV_LOAD_PATH_SETTINGS_TAG];

    m_lastCsvPath = v.toString();
  }

  if (map.contains(LAST_CHEMSTATION_DLG_SIZE_TAG)) {
    const QVariant &v = map[LAST_CHEMSTATION_DLG_SIZE_TAG];

    m_lastChemStationDlgSize = v.toSize();
  }

  /* Act upon the loaded settings where necessary */
  if (m_lastChemStationPath.length() > 0) {
    if (!isDirectoryUsable(m_lastChemStationPath))
      m_lastChemStationPath = defaultPath();

    m_loadChemStationDataDlg->expandToPath(m_lastChemStationPath);
  }
}

void DataFileLoader::onLoadDataFile(const DataFileLoaderMsgs::LoadableFileTypes type)
{
  switch (type) {
  case DataFileLoaderMsgs::LoadableFileTypes::CHEMSTATION:
    loadChemStationFile();
    break;
  case DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_CLIPBOARD:
    loadCsvFile(true);
    break;
  case DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_FILE:
    loadCsvFile(false);
    break;
  default:
    break;
  }
}

QVariant DataFileLoader::saveUserSettings() const
{
  EMT::StringVariantMap map;

  map[LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG] = m_lastChemStationPath;
  map[LAST_CSV_LOAD_PATH_SETTINGS_TAG] = m_lastCsvPath;
  map[LAST_CHEMSTATION_DLG_SIZE_TAG] = m_lastChemStationDlgSize;

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

