#include "datafileloader.h"
#include "custommetatypes.h"
#include "netcdffileloader.h"
#include "gui/loadchemstationdatadialog.h"
#include "gui/loadcsvfiledialog.h"
#include <QFileDialog>
#include <QMessageBox>

const QString DataFileLoader::LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG("LastChemStationLoadPath");
const QString DataFileLoader::LAST_CSV_LOAD_PATH_SETTINGS_TAG("LastCsvLoadPath");
const QString DataFileLoader::LAST_NETCDF_LOAD_PATH_SETTINGS_TAG("LastNetCDFLoadPath");
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
  m_lastNetCDFPath(QDir::homePath()),
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
    return tr("Analog input");
    break;
  case ChemStationFileLoader::Type::CE_CCD:
    return tr("Conductivity");
    break;
  case ChemStationFileLoader::Type::CE_CURRENT:
    return tr("Current");
    break;
  case ChemStationFileLoader::Type::CE_DAD:
    return tr("Absorbance");
    break;
  case ChemStationFileLoader::Type::CE_POWER:
    return tr("Power");
    break;
  case ChemStationFileLoader::Type::CE_PRESSURE:
    return tr("Pressure");
    break;
  case ChemStationFileLoader::Type::CE_TEMPERATURE:
    return tr("Temperature");
    break;
  case ChemStationFileLoader::Type::CE_VOLTAGE:
    return tr("Voltage");
    break;
  case ChemStationFileLoader::Type::CE_UNKNOWN:
  default:
    return tr("Unknown");
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

void DataFileLoader::loadCsvData(const CsvFileLoader::Data &csvData, const QString &file, const LoadCsvFileDialog::Parameters &p)
{
  QString xType;
  QString yType;
  QString xUnit;
  QString yUnit;
  QString fileName;

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

  if (file.length() > 0)
    fileName = QFileInfo(file).fileName();

  emit dataLoaded(data, file, fileName);
}

void DataFileLoader::loadCsvFromClipboard()
{
  while (true) {
    CsvFileLoader::Parameters readerParams = makeCsvLoaderParameters();
    if (!readerParams.isValid)
      return;

    CsvFileLoader::Data csvData = CsvFileLoader::readClipboard(readerParams);
    if (!csvData.isValid()) {
      readerParams = CsvFileLoader::Parameters();
      continue;
    }

    loadCsvData(csvData, QString(), m_loadCsvFileDlg->parameters());
    return;
  }
}

void DataFileLoader::loadCsvFromFile()
{
  QStringList files;
  QFileDialog openDlg(nullptr, tr("Pick a text data file"), m_lastCsvPath);

  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFiles);

  if (openDlg.exec() != QDialog::Accepted)
    return;

  files = openDlg.selectedFiles();
  if (files.length() < 1)
    return;

  CsvFileLoader::Parameters readerParams;

  for (const QString &f : files) {
    while (true) {
      if (!readerParams.isValid) {
        readerParams = makeCsvLoaderParameters();
        if (!readerParams.isValid)
          break;
      }

      CsvFileLoader::Data csvData = CsvFileLoader::readFile(f, readerParams);
      if (!csvData.isValid()) {
        readerParams = CsvFileLoader::Parameters();
        continue;
      }

      m_lastCsvPath = f;
      loadCsvData(csvData, f, m_loadCsvFileDlg->parameters());
      break;
    }
  }
}

void DataFileLoader::loadNetCDFFile()
{
  QStringList files;
  QFileDialog openDlg(nullptr, tr("Pick a NetCDF file"), m_lastNetCDFPath);

  openDlg.setNameFilter("NetCDF file (*.cdf *.CDF)");
  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFiles);

  if  (openDlg.exec() != QDialog::Accepted)
      return;

  files = openDlg.selectedFiles();
  if (files.length() < 1)
      return;

  const QString file = files.at(0);

  try {
    NetCDFFileLoader::Data data = NetCDFFileLoader::load(file);

    QVector<QPointF> qData;
    qData.reserve(data.scans.size());

    for (const auto &point : data.scans)
      qData.append(QPointF(std::get<0>(point), std::get<1>(point)));

    std::shared_ptr<Data> globData = std::shared_ptr<Data>(new Data(qData,
                                                                    "Time", data.xUnits,
                                                                    "Detector", data.yUnits));

    QString fileName = QFileInfo(file).fileName();

    m_lastNetCDFPath = file;
    emit dataLoaded(globData, file, fileName);
  } catch (std::runtime_error &err) {
    QMessageBox::warning(nullptr, tr("Failed to load NetCDF file"), err.what());
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

  if (map.contains(LAST_NETCDF_LOAD_PATH_SETTINGS_TAG)) {
    const QVariant &v = map[LAST_NETCDF_LOAD_PATH_SETTINGS_TAG];

    m_lastNetCDFPath = v.toString();
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

CsvFileLoader::Parameters DataFileLoader::makeCsvLoaderParameters()
{
  while (true) {
    if (m_loadCsvFileDlg->exec() != QDialog::Accepted)
      return CsvFileLoader::Parameters();

    const LoadCsvFileDialog::Parameters p = m_loadCsvFileDlg->parameters();

    if (p.delimiter.length() != 1 && (p.delimiter.compare("\\t") != 0)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter must be a single character or '\\t' to represent TAB."));
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

    QChar delimiter;
    if (p.delimiter.compare("\\t") == 0)
      delimiter = '\t';
    else
      delimiter = p.delimiter.at(0);

    const QByteArray &bom = p.readBom == true ? CsvFileLoader::SUPPORTED_ENCODINGS[p.encodingId].bom : QByteArray();

    return CsvFileLoader::Parameters(delimiter, p.decimalSeparator,
                                     p.xColumn, p.yColumn,
                                     p.header != LoadCsvFileDialog::HeaderHandling::NO_HEADER,
                                     p.linesToSkip,
                                     p.encodingId, p.readBom);
  }
}

void DataFileLoader::onLoadDataFile(const DataFileLoaderMsgs::LoadableFileTypes type)
{
  switch (type) {
  case DataFileLoaderMsgs::LoadableFileTypes::CHEMSTATION:
    loadChemStationFile();
    break;
  case DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_CLIPBOARD:
    loadCsvFromClipboard();
    break;
  case DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_FILE:
    loadCsvFromFile();
    break;
  case DataFileLoaderMsgs::LoadableFileTypes::NETCDF_FILE:
    loadNetCDFFile();
  default:
    break;
  }
}

QVariant DataFileLoader::saveUserSettings() const
{
  EMT::StringVariantMap map;

  map[LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG] = m_lastChemStationPath;
  map[LAST_CSV_LOAD_PATH_SETTINGS_TAG] = m_lastCsvPath;
  map[LAST_NETCDF_LOAD_PATH_SETTINGS_TAG] = m_lastNetCDFPath;
  map[LAST_CHEMSTATION_DLG_SIZE_TAG] = m_lastChemStationDlgSize;

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

