#include "netcdfsupport.h"
#include <QFileDialog>
#include <QMessageBox>
#include "netcdffileloader.h"
#include "../../efgloader-core/common/backendhelpers_p.h"

namespace backend {

Identifier NetCDFSupport::s_identifier{"NetCDF (Common Data Format) file format support", "NetCDF", "NetCDF", {}};
NetCDFSupport *NetCDFSupport::s_me{nullptr};

LoaderBackend::~LoaderBackend()
{
}

NetCDFSupport::NetCDFSupport()
{
}

NetCDFSupport::~NetCDFSupport()
{
}

void NetCDFSupport::destroy()
{
  delete s_me;
}

Identifier NetCDFSupport::identifier() const
{
  return s_identifier;
}

NetCDFSupport *NetCDFSupport::instance()
{
  if (s_me == nullptr)
    s_me = new (std::nothrow) NetCDFSupport{};

  return s_me;
}

std::vector<Data> NetCDFSupport::load(const int option)
{
  (void)option;

  return loadInternal("");
}

std::vector<Data> NetCDFSupport::loadHint(const std::string &hintPath, const int option)
{
  (void)option;

  return loadInternal(QString::fromStdString(hintPath));
}

std::vector<Data> NetCDFSupport::loadPath(const std::string &path, const int option)
{
  (void)option;

  try {
    return std::vector<Data>{loadOneFile(QString::fromStdString(path))};
  } catch (std::runtime_error &ex) {
    QMessageBox::warning(nullptr, QObject::tr("Failed to load NetCDF file"), QString(ex.what()));
    return std::vector<Data>{};
  }
}

std::vector<Data> NetCDFSupport::loadInternal(const QString &path)
{
  QStringList files{};
  QFileDialog openDlg{nullptr, QObject::tr("Pick a NetCDF file"), path};
  std::vector<Data> retData{};

  openDlg.setNameFilter("NetCDF file (*.cdf *.CDF)");
  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFiles);

  BackendHelpers::showWindowOnTop(&openDlg);
  if  (openDlg.exec() != QDialog::Accepted)
    return std::vector<Data>{};

  files = openDlg.selectedFiles();
  if (files.length() < 1)
    return std::vector<Data>{};

  for (const QString &filePath : files) {
    try {
      retData.emplace_back(loadOneFile(filePath));
    } catch (std::runtime_error &ex) {
      QMessageBox::warning(nullptr, QObject::tr("Failed to load NetCDF file"), QString{ex.what()});
    }
  }

  return retData;
}

Data NetCDFSupport::loadOneFile(const QString &filePath)
{
  NetCDFFileLoader::Data data = NetCDFFileLoader::load(filePath);
  QString fileName = QFileInfo{filePath}.fileName();

  return Data{fileName.toStdString(), filePath.toStdString(),
              "Time", "Signal",
              std::move(data.xUnits),
              std::move(data.yUnits),
              std::move(data.scans)};
}

LoaderBackend * initialize()
{
  return NetCDFSupport::instance();
}

} // namespace backend
