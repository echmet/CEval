#include "hpcssupport.h"
#include <QFile>
#include <QMessageBox>
#include "loadchemstationdatadialog.h"
#include "../../efgloader-core/common/backendhelpers_p.h"
#include "../../efgloader-core/common/threadeddialog.h"

namespace backend {

bool isDirectoryUsable(const QString &path)
{
  const QDir dir(path);

  return (dir.exists() && dir.isReadable());
}

class LoadChemStationDataThreadedDialog : public ThreadedDialog<LoadChemStationDataDialog>
{
public:
  LoadChemStationDataThreadedDialog(UIBackend *backend, const QString &path, const QSize &dlgSize) :
    ThreadedDialog<LoadChemStationDataDialog>{
      backend,
      [this, backend]() {
        auto dlg = new LoadChemStationDataDialog{backend};

        if (isDirectoryUsable(this->m_path))
          dlg->expandToPath(this->m_path);

        if (this->m_dlgSize.width() > 0 && m_dlgSize.height() > 0)
          dlg->resize(this->m_dlgSize);

        BackendHelpers::showWindowOnTop(dlg);
        return dlg;
      },
    },
    m_path{path},
    m_dlgSize{dlgSize}
  {}

private:
  const QString m_path;
  const QSize m_dlgSize;
};


HPCSSupport *HPCSSupport::s_me{nullptr};
const Identifier HPCSSupport::s_identifier{"Agilent(HP) ChemStation file format support", "ChemStation", "HPCS", {}};

LoaderBackend::~LoaderBackend()
{
}

HPCSSupport::HPCSSupport(UIBackend *backend) :
  m_uiBackend{backend},
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
}

HPCSSupport::~HPCSSupport()
{
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

HPCSSupport * HPCSSupport::instance(UIBackend *backend)
{
  if (s_me == nullptr) {
    try {
      s_me = new HPCSSupport{backend};
    } catch (...) {
      s_me = nullptr;
    }
  }

  return s_me;
}

LoadChemStationDataThreadedDialog * HPCSSupport::makeLoadDialog(const QString &path)
{
  m_dlgSizeLock.lock();
  auto dlg = new LoadChemStationDataThreadedDialog{m_uiBackend, path, m_lastChemStationDlgSize};
  m_dlgSizeLock.unlock();

  return dlg;
}

std::vector<Data> HPCSSupport::load(const int option)
{
  Q_UNUSED(option);

  m_lastPathLock.lock();
  const QString path = m_lastChemStationPath;
  m_lastPathLock.unlock();

  auto *dlg = makeLoadDialog(path);
  const auto ret = loadInteractive(dlg);
  delete dlg;

  return ret;
}

std::vector<Data> HPCSSupport::loadHint(const std::string &hintPath, const int option)
{
  Q_UNUSED(option);

  auto *dlg = makeLoadDialog(QString::fromStdString(hintPath));
  const auto ret = loadInteractive(dlg);
  delete dlg;

  return ret;
}

std::vector<Data> HPCSSupport::loadInteractive(LoadChemStationDataThreadedDialog *dlg)
{
  std::vector<Data> dataVec{};

  if (dlg->execute() != QDialog::Accepted)
    return dataVec;

  const auto loadInfo = dlg->dialog()->loadInfo();

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

  m_dlgSizeLock.lock();
  m_lastChemStationDlgSize = dlg->dialog()->size();
  m_dlgSizeLock.unlock();
  m_lastPathLock.lock();
  m_lastChemStationPath = loadInfo.path;
  m_lastPathLock.unlock();

  return dataVec;
}

std::vector<Data> HPCSSupport::loadPath(const std::string &path, const int option)
{
  Q_UNUSED(option);

  return {loadChemStationFileSingle(QString::fromStdString(path))};
}

Data HPCSSupport::loadChemStationFileSingle(const QString &path)
{
  ChemStationFileLoader::Data chData = ChemStationFileLoader::loadFile(m_uiBackend, path, true);

  if (!chData.isValid())
    return Data{};

  QDir dir(path);
  dir.cdUp();
  m_lastChemStationPath = dir.path();

  std::vector<std::tuple<double, double>> datapoints;
  for (const auto &datapoint : chData.data)
    datapoints.emplace_back(std::make_tuple(datapoint.x(), datapoint.y()));

  Data data{QFileInfo(path).fileName().toStdString(),
            "",
            path.toStdString(),
            "Time",
            chemStationTypeToString(chData.type),
            "minute",
            chData.yUnits.toStdString(),
            std::move(datapoints)};

  return data;
}

void HPCSSupport::loadChemStationFileMultipleDirectories(std::vector<Data> &dataVec, const QStringList &dirPaths, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(m_uiBackend, dirPaths, filter);

  for (const QString &path : files)
    dataVec.emplace_back(loadChemStationFileSingle(path));
}

void HPCSSupport::loadChemStationFileWholeDirectory(std::vector<Data> &dataVec, const QString &path, const ChemStationBatchLoader::Filter &filter)
{
  QStringList files = ChemStationBatchLoader::getFilesList(m_uiBackend, path, filter);

  for (const QString &filePath : files)
    dataVec.emplace_back(loadChemStationFileSingle(filePath));
}

LoaderBackend * initialize(UIBackend *backend)
{
  return HPCSSupport::instance(backend);
}

} // namespace plugin
