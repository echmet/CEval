#include "custommetatypes.h"
#include "dataexporter/exporterglobals.h"
#include "softwareupdater.h"
#include "datahash.h"
#include "softwareupdateresult.h"

void EMT::registerAll()
{
  qRegisterMetaType<DataExporter::Globals::DataArrangement>();
  qRegisterMetaType<EMT::SeriesSettingsMap>();
  qRegisterMetaType<DataHash>("DataHash");
  qRegisterMetaType<SoftwareUpdateResult>("SoftwareUpdateResult");
}
