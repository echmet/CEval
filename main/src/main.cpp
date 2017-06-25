#include "gui/evalmainwindow.h"
#include "custommetatypes.h"
#include "cevalcrashhandler.h"
#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "globals.h"
#include "hvlcalculator.h"
#include "softwareupdater.h"
#include <QApplication>
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
static const QString NUM_FORMAT_SETTINGS_TAG("NumFormat");
static const QString SOFTWARE_UPDATER_SETTINGS_TAG("SoftwareUpdater");
static const QString ROOT_SETTINGS_TAG("Root");

QString configFileName()
{
  return Globals::SOFTWARE_NAME + QString(".conf");
}

void loadUserSettings(DataAccumulator *dac, SoftwareUpdater *updater)
{
  QSettings s(configFileName(), QSettings::IniFormat);

  QVariant root = s.value(ROOT_SETTINGS_TAG);
  if (!root.canConvert<QMap<QString, QVariant>>())
    return;

  QMap<QString, QVariant> rootMap = root.value<EMT::StringVariantMap>();

  if (rootMap.contains(DAC_SETTINGS_TAG))
    dac->loadUserSettings(rootMap[DAC_SETTINGS_TAG]);

  if (rootMap.contains(NUM_FORMAT_SETTINGS_TAG))
    DoubleToStringConvertor::loadUserSettings(rootMap[NUM_FORMAT_SETTINGS_TAG]);

  if (rootMap.contains(SOFTWARE_UPDATER_SETTINGS_TAG))
    updater->loadUserSettings(rootMap[SOFTWARE_UPDATER_SETTINGS_TAG]);
}

#if defined(Q_OS_UNIX)
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

void setupBindings(EvalMainWindow *w, DataAccumulator *dac)
{
  w->connectToAccumulator(dac);
}

void saveUserSettings(DataAccumulator *dac, SoftwareUpdater *updater)
{
  QSettings s(configFileName(), QSettings::IniFormat);

  QMap<QString, QVariant> map;

  map.insert(DAC_SETTINGS_TAG, dac->saveUserSettings());
  map.insert(NUM_FORMAT_SETTINGS_TAG, DoubleToStringConvertor::saveUserSettings());
  map.insert(SOFTWARE_UPDATER_SETTINGS_TAG, updater->saveUserSettings());

  s.setValue(ROOT_SETTINGS_TAG, map);
}

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

  DoubleToStringConvertor::initialize();

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
  loadUserSettings(dac, updater);

  setDefaultState(w);

  CEvalCrashHandler::checkForCrash();
  dac->checkForCrashRecovery();

  updater->checkAutomatically();
  w->connectToSoftwareUpdater(updater);

  aRet =  a.exec();

  saveUserSettings(dac, updater);

out:
  CEvalCrashHandler::uninstallCrashHandler();

  return aRet;
}
