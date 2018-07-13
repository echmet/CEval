#include "gui/evalmainwindow.h"
#include "custommetatypes.h"
#include "cevalcrashhandler.h"
#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "globals.h"
#include "hvlcalculator.h"
#include "softwareupdater.h"
#include "efg/efgloaderinterface.h"
#include "efg/efgloaderwatcher.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <thread>
#include <omp.h>

#if defined(Q_OS_UNIX)
#include <cstdlib>
#include <stdexcept>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include <tchar.h>
#endif


static const QString DAC_SETTINGS_TAG("DataAccumulator");
static const QString EFG_LOADER_INTERFACE_SETTINGS_TAG("EFGLoaderInterface");
static const QString NUM_FORMAT_SETTINGS_TAG("NumFormat");
static const QString SOFTWARE_UPDATER_SETTINGS_TAG("SoftwareUpdater");
static const QString ROOT_SETTINGS_TAG("Root");
static const QString EDII_SERVICE_PATH_TAG("EDIIServicePath");

static
void applyUserSettings(const QMap<QString, QVariant> &rootMap, DataAccumulator *dac, SoftwareUpdater *updater)
{
  if (rootMap.contains(DAC_SETTINGS_TAG))
    dac->loadUserSettings(rootMap[DAC_SETTINGS_TAG]);

  if (rootMap.contains(NUM_FORMAT_SETTINGS_TAG))
    DoubleToStringConvertor::loadUserSettings(rootMap[NUM_FORMAT_SETTINGS_TAG]);

  if (rootMap.contains(SOFTWARE_UPDATER_SETTINGS_TAG))
    updater->loadUserSettings(rootMap[SOFTWARE_UPDATER_SETTINGS_TAG]);

  if (rootMap.contains(EFG_LOADER_INTERFACE_SETTINGS_TAG))
    EFGLoaderInterface::instance().loadUserSettings(rootMap[EFG_LOADER_INTERFACE_SETTINGS_TAG]);
}

static
void checkEDIIServicePath(QString &ediiServicePath)
{
  if (efg::EFGLoaderWatcher::isServicePathValid(ediiServicePath))
    return;

  QFileDialog dlg(nullptr, "Set path to ECHMET Data Import Infrastructure service", ediiServicePath);

  dlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);

  while (true) {
    if (dlg.exec() != QDialog::Accepted)
      return;

    auto path = dlg.selectedFiles()[0];
    if (efg::EFGLoaderWatcher::isServicePathValid(path)) {
      ediiServicePath = std::move(path);
      return;
    }

    QMessageBox mbox(QMessageBox::Warning,
                     QObject::tr("ECHMET Data Import Infrastructure error"),
                     QObject::tr("Given path does not appear to contain EDDI service executable. Please enter a valid path."));
    mbox.exec();
  }
}

static
QString configFileName()
{
  return Globals::SOFTWARE_NAME + QString(".conf");
}

static
QMap<QString, QVariant> loadUserSettings()
{
  QSettings s(configFileName(), QSettings::IniFormat);

  QVariant root = s.value(ROOT_SETTINGS_TAG);
  if (!root.canConvert<EMT::StringVariantMap>())
    return {};

  return root.value<EMT::StringVariantMap>();
}

#if defined(Q_OS_UNIX)
static
void setOpenMPThreads()
{
  int hwThreads;
  const char *ompThreads = getenv("OMP_NUM_THREADS");

  if (ompThreads == nullptr)
    hwThreads = static_cast<int>(std::thread::hardware_concurrency());
  else {
    try {
      hwThreads = std::stoi(ompThreads);
    } catch (std::invalid_argument &) {
      hwThreads = 0;
    } catch (std::out_of_range &) {
      hwThreads = 0;
    }
  }

  if (hwThreads > 0)
    omp_set_num_threads(hwThreads);
}
#elif defined(Q_OS_WIN)
static
void setOpenMPThreads()
{
  int hwThreads;
  DWORD ret;
  LPTSTR ompThreads;
  HANDLE heap = GetProcessHeap();

  if (heap == NULL)
      return;

  ompThreads = static_cast<LPTSTR>(HeapAlloc(heap, HEAP_ZERO_MEMORY, 32767));
  if (ompThreads == nullptr)
    return;

  ret = GetEnvironmentVariable(TEXT("OMP_NUM_THREADS"), ompThreads, 32767);
  if (ret <= 0)
    hwThreads = static_cast<int>(std::thread::hardware_concurrency());
  else {
  #if defined UNICODE || defined _UNICODE
    hwThreads = wcstol(ompThreads, nullptr, 10);
  #else
    hwThreads = strtol(ompThreads, nullptr, 10);
  #endif

    if (hwThreads == LONG_MIN || hwThreads == LONG_MAX)
      hwThreads = 0;
  }

  HeapFree(heap, 0, ompThreads);

  if (hwThreads > 0)
    omp_set_num_threads(hwThreads);
}
#else
void setOpenMPThreads()
{
  return; /* Unknown architecture, do nothing */
}
#endif

static
void setupBindings(EvalMainWindow *w, DataAccumulator *dac)
{
  w->connectToAccumulator(dac);
}

static
void saveUserSettings(DataAccumulator *dac, SoftwareUpdater *updater, const QString &ediiServicePath)
{
  QSettings s(configFileName(), QSettings::IniFormat);

  EMT::StringVariantMap map;

  map.insert(DAC_SETTINGS_TAG, dac->saveUserSettings());
  map.insert(NUM_FORMAT_SETTINGS_TAG, DoubleToStringConvertor::saveUserSettings());
  map.insert(SOFTWARE_UPDATER_SETTINGS_TAG, updater->saveUserSettings());
  map.insert(EFG_LOADER_INTERFACE_SETTINGS_TAG, EFGLoaderInterface::instance().saveUserSettings());
  map.insert(EDII_SERVICE_PATH_TAG, ediiServicePath);

  s.setValue(ROOT_SETTINGS_TAG, map);
  s.sync();
}

static
void setDefaultState(EvalMainWindow *w)
{
  w->setDefaultState();
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  EvalMainWindow *w;
  DataAccumulator *dac;
  SoftwareUpdater *updater;
  int aRet;
  QString ediiServicePath;

  if (!CEvalCrashHandler::installCrashHandler()) {
    QMessageBox::warning(nullptr, QObject::tr("Unable to install crash handler"),
                         QObject::tr("CEval was unable to install crash handler. You will not receive a detailed diagnostic"
                                     "output in an event of unhandled internal error."));
  }

  setOpenMPThreads();

  QCoreApplication::setOrganizationName(Globals::ORG_NAME);
  QCoreApplication::setApplicationName(Globals::SOFTWARE_NAME);
  QCoreApplication::setApplicationVersion(Globals::VERSION_STRING());

  EMT::registerAll();
  EFGLoaderInterface::registerMetaTypes();

  DoubleToStringConvertor::initialize();

  const auto cfg = loadUserSettings();

  if (cfg.contains(EDII_SERVICE_PATH_TAG))
    ediiServicePath = cfg[EDII_SERVICE_PATH_TAG].toString();

  checkEDIIServicePath(ediiServicePath);
  try {
    EFGLoaderInterface::initialize(ediiServicePath);
  } catch (const std::runtime_error &) {
    QMessageBox mbox(QMessageBox::Critical,
                     QObject::tr("ECHMET Data Import Infrastructure error"),
                     QString(QObject::tr("%1 failed to connect to EDII service. The program will terminate.").arg(Globals::SOFTWARE_NAME)));
    mbox.exec();

    return EXIT_FAILURE;
  }

  try {
    w = new EvalMainWindow(nullptr);
    dac = new DataAccumulator(w->plot(), nullptr);
    updater = new SoftwareUpdater(nullptr);
  } catch (std::exception &ex) {
    QMessageBox::critical(nullptr, QObject::tr("Initialization"), QString(QObject::tr("Unable to allocate basic data structures\n%1")).arg(ex.what()));
    aRet = EXIT_FAILURE;
    goto out;
  } catch (...) {
    QMessageBox::critical(nullptr, QObject::tr("Initialization"), QObject::tr("Unable to allocate basic data structures"));
    aRet = EXIT_FAILURE;
    goto out;
  }

  HVLCalculator::initialize();

  setupBindings(w, dac);

  w->show();
  dac->announceDefaultState();

  applyUserSettings(cfg, dac, updater);

  setDefaultState(w);

  CEvalCrashHandler::checkForCrash();
  dac->checkForCrashRecovery();

  updater->checkAutomatically();
  w->connectToSoftwareUpdater(updater);
  EFGLoaderInterface::instance().retrieveSupportedFileFormats();

  aRet =  a.exec();

  saveUserSettings(dac, updater, ediiServicePath);

out:
  CEvalCrashHandler::uninstallCrashHandler();

  return aRet;
}
