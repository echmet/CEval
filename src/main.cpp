#include "gui/evalmainwindow.h"
#include "custommetatypes.h"
#include "crashhandler.h"
#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "globals.h"
#include "hvlcalculator.h"
#include <QApplication>
#include <QMessageBox>
#include <QSettings>

static const QString DAC_SETTINGS_TAG("DataAccumulator");
static const QString NUM_FORMAT_SETTINGS_TAG("NumFormat");
static const QString ROOT_SETTINGS_TAG("Root");

QString configFileName()
{
  return Globals::SOFTWARE_NAME + QString(".conf");
}

void loadUserSettings(DataAccumulator *dac)
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
}

void setupBindings(EvalMainWindow *w, DataAccumulator *dac)
{
  w->connectToAccumulator(dac);
}

void saveUserSettings(DataAccumulator *dac)
{
  QSettings s(configFileName(), QSettings::IniFormat);

  QMap<QString, QVariant> map;

  map.insert(DAC_SETTINGS_TAG, dac->saveUserSettings());
  map.insert(NUM_FORMAT_SETTINGS_TAG, DoubleToStringConvertor::saveUserSettings());

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
  int aRet;

  QCoreApplication::setOrganizationName(Globals::ORG_NAME);
  QCoreApplication::setApplicationName(Globals::SOFTWARE_NAME);
  QCoreApplication::setApplicationVersion(Globals::VERSION_STRING());

  EMT::registerAll();

  CrashHandler::install();

  try {
    w = new EvalMainWindow(nullptr);
    dac = new DataAccumulator(w->plot(), nullptr);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, QObject::tr("Insufficient memory"), QObject::tr("Unable to allocate basic data structures"));
    aRet = EXIT_FAILURE;
    goto out;
  }
  HVLCalculator::initialize();
  DoubleToStringConvertor::initialize();

  setupBindings(w, dac);

  w->show();
  loadUserSettings(dac);

  setDefaultState(w);

  dac->checkForCrashRecovery();

  aRet =  a.exec();

  saveUserSettings(dac);

out:
  CrashHandler::uninstall();

  return aRet;
}
