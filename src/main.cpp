#include "gui/evalmainwindow.h"
#include "cevalconfig.h"
#include "custommetatypes.h"
#include "cevalcrashhandler.h"
#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "globals.h"
#include "hvlcalculator.h"
#include "softwareupdater.h"
#include "efg/efgloaderinterface.h"
#include "efg/efgloaderwatcher.h"
#include "gui/ediinotfounddialog.h"
#include "gui/selectediipath.h"

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

static
void applyUserSettings(DataAccumulator *dac, SoftwareUpdater *updater)
{
  QVariant v;

  v = CEvalConfig::value(CEvalConfig::DAC_SETTINGS_TAG);
  if (v.isValid())
    dac->loadUserSettings(v);

  v = CEvalConfig::value(CEvalConfig::NUM_FORMAT_SETTINGS_TAG);
  if (v.isValid())
    DoubleToStringConvertor::loadUserSettings(v);

  v = CEvalConfig::value(CEvalConfig::SOFTWARE_UPDATER_SETTINGS_TAG);
  if (v.isValid())
    updater->loadUserSettings(v);

  v = CEvalConfig::value(CEvalConfig::EFG_LOADER_INTERFACE_SETTINGS_TAG);
  if (v.isValid())
    EFGLoaderInterface::instance().loadUserSettings(v);
}

static
void checkEDIIServicePath(QString &ediiServicePath)
{
  if (efg::EFGLoaderWatcher::isServicePathValid(ediiServicePath))
    return;

  EDIINotFoundDialog infoDlg;
  if (infoDlg.exec() == QDialog::Rejected) {
    ediiServicePath = QString("%1/EDII/%2/%3").arg(qApp->applicationDirPath())
                                              .arg(efg::EFGLoaderWatcher::s_EFGLoaderPathPrefix)
                                              .arg(efg::EFGLoaderWatcher::s_EFGLoaderBinaryName);
    return;
  }

  ediiServicePath = SelectEDIIPath::browseToEDII(ediiServicePath);
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
  CEvalConfig::setValue(CEvalConfig::DAC_SETTINGS_TAG, dac->saveUserSettings());
  CEvalConfig::setValue(CEvalConfig::NUM_FORMAT_SETTINGS_TAG, DoubleToStringConvertor::saveUserSettings());
  CEvalConfig::setValue(CEvalConfig::SOFTWARE_UPDATER_SETTINGS_TAG, updater->saveUserSettings());
  CEvalConfig::setValue(CEvalConfig::EFG_LOADER_INTERFACE_SETTINGS_TAG, EFGLoaderInterface::instance().saveUserSettings());
  CEvalConfig::setValue(CEvalConfig::EDII_SERVICE_PATH_TAG, ediiServicePath);

  CEvalConfig::save();
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
                         QString(QObject::tr("%1 was unable to install crash handler. You will not receive a detailed diagnostic "
                                             "output in an event of unhandled internal error.")).arg(Globals::SOFTWARE_NAME));
  }

  setOpenMPThreads();

  QCoreApplication::setOrganizationName(Globals::ORG_NAME);
  QCoreApplication::setApplicationName(Globals::SOFTWARE_NAME);
  QCoreApplication::setApplicationVersion(Globals::VERSION_STRING());

  EMT::registerAll();
  EFGLoaderInterface::registerMetaTypes();

  DoubleToStringConvertor::initialize();

  if (!CEvalConfig::initialize()) {
    QMessageBox mbox(QMessageBox::Critical, QObject::tr("Configuration error"),
                     QString(QObject::tr("%1 failed to initialize user configuration handler. The program will terminate")).arg(Globals::SOFTWARE_NAME));

    mbox.exec();

    return EXIT_FAILURE;
  }

  ediiServicePath = CEvalConfig::value(CEvalConfig::EDII_SERVICE_PATH_TAG).toString();

  checkEDIIServicePath(ediiServicePath);
  try {
    EFGLoaderInterface::initialize(ediiServicePath);
  } catch (const std::runtime_error &ex) {
    QMessageBox mbox(QMessageBox::Critical,
                     QObject::tr("ECHMET Data Import Infrastructure error"),
                     QString(QObject::tr("%1 failed to connect to EDII service. The program will terminate.\n"
                                         "Error reported:\n%2")
                             .arg(Globals::SOFTWARE_NAME))
                             .arg(ex.what()));
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

  applyUserSettings(dac, updater);

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
