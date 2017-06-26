#include "hpcssupport.h"
#include <QFile>
#include <QMessageBox>
#include "loadchemstationdatadialog.h"

namespace backend {

HPCSSupport *HPCSSupport::s_me{nullptr};
Identifier HPCSSupport::s_identifier{"Agilent(HP) ChemStation file format support", "ChemStation", "HPCS", {}};

LoaderBackend::~LoaderBackend()
{
}

HPCSSupport::HPCSSupport() :
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
  m_loadChemStationDataDlg = new LoadChemStationDataDialog{};
}

HPCSSupport::~HPCSSupport()
{
  delete m_loadChemStationDataDlg;
}

std::string HPCSSupport::chemStationTypeToString(const ChemStationFileLoader::Type type)
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

QString HPCSSupport::defaultPath() const
{
  for (const QString &path : m_defaultPathsToTry) {
    QDir dir(path);

    if (dir.exists() && dir.isReadable())
      return path;
  }

  return "";
}

void HPCSSupport::destroy()
{
  delete s_me;
}

Identifier HPCSSupport::identifier() const
{
  return s_identifier;
}

HPCSSupport * HPCSSupport::instance()
{
  if (s_me == nullptr) {
    try {
      s_me = new HPCSSupport{};
    } catch (...) {
      s_me = nullptr;
    }
  }

  return s_me;
}

bool HPCSSupport::isDirectoryUsable(const QString &path) const
{
  const QDir dir(path);

  return (dir.exists() && dir.isReadable());
}

std::vector<Data> HPCSSupport::load(const int option)
{
  Q_UNUSED(option);
  int ret;

  if (m_lastChemStationDlgSize.width() > 0 && m_lastChemStationDlgSize.height() > 0)
      m_loadChemStationDataDlg->resize(m_lastChemStationDlgSize);

  if (!isDirectoryUsable(m_lastChemStationPath))
    m_lastChemStationPath = defaultPath();

  m_loadChemStationDataDlg->expandToPath(m_lastChemStationPath);

  ret = m_loadChemStationDataDlg->exec();
  m_lastChemStationDlgSize = m_loadChemStationDataDlg->size();

  if (ret != QDialog::Accepted)
    return std::vector<Data>{};

  LoadChemStationDataDialog::LoadInfo loadInfo = m_loadChemStationDataDlg->loadInfo();

  std::vector<Data> dataVec;
  switch (loadInfo.loadingMode) {
  case LoadChemStationDataDialog::LoadingMode::SINGLE_FILE:
    dataVec.emplace_back(loadChemStationFileSingle(loadInfo.path));
    break;
  case LoadChemStationDataDialog::LoadingMode::MULTIPLE_DIRECTORIES:
    loadChemStationFileMultipleDirectories(dataVec, loadInfo.dirPaths, loadInfo.filter);
    break;
  case LoadChemStationDataDialog::LoadingMode::WHOLE_DIRECTORY:
    loadChemStationFileWholeDirectory(dataVec, loadInfo.path, loadInfo.filter);
    break;
  }

  return dataVec;
}

std::vector<Data> HPCSSupport::loadHint(const std::string &hintPath, const int option)
{
  const QString qHintPath(hintPath.c_str());

  if (isDirectoryUsable(qHintPath))
    m_lastChemStationPath = qHintPath;

  return load(option);
}

std::vector<Data> HPCSSupport::loadPath(const std::string &path, const int option)
{
  Q_UNUSED(option);
  Data d = loadChemStationFileSingle(QString::fromStdString(path));

  return std::vector<Data>{d};
}

Data HPCSSupport::loadChemStationFileSingle(const QString &path)
{
  ChemStationFileLoader::Data chData = ChemStationFileLoader::loadFile(path, true);

  if (!chData.isValid())
    return Data{};

  QDir dir(path);
  dir.cdUp();
  m_lastChemStationPath = dir.path();

  Data data{};

  data.name = QFileInfo(path).fileName().toStdString();
  data.path = path.toStdString();
  data.xDescription = "Time";
  data.yDescription = chemStationTypeToString(chData.type);
  data.xUnit = "minute";
  data.yUnit = chData.yUnits.toStdString();

  for (const auto &datapoint : chData.data)
    data.datapoints.emplace_back(std::make_tuple(datapoint.x(), datapoint.y()));

  return data;
}

void HPCSSupport::loadChemStationFileMultipleDirectories(std::vector<Data> &dataVec, const QStringList &dirPaths, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(dirPaths, filter);

  for (const QString &path : files)
    dataVec.emplace_back(loadChemStationFileSingle(path));
}

void HPCSSupport::loadChemStationFileWholeDirectory(std::vector<Data> &dataVec, const QString &path, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(path, filter);

  for (const QString &filePath : files)
    dataVec.emplace_back(loadChemStationFileSingle(filePath));
}

LoaderBackend * initialize()
{
  return HPCSSupport::instance();
}

} // namespace plugin
