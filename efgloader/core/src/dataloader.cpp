#include "dataloader.h"
#include "common/backendinterface.h"
#include <QDir>
#include <QLibrary>

#include <iostream>

FileFormatInfo::FileFormatInfo() :
  longDescription(""),
  shortDescription(""),
  tag(""),
  loadOptions(QVector<QString>{})
{
}

FileFormatInfo::FileFormatInfo(const QString &longDescription, const QString &shortDescription, const QString &tag, const QVector<QString> &loadOptions) :
  longDescription(longDescription),
  shortDescription(shortDescription),
  tag(tag),
  loadOptions(loadOptions)
{
}

FileFormatInfo::FileFormatInfo(const FileFormatInfo &other) :
  longDescription(other.longDescription),
  shortDescription(other.shortDescription),
  tag(other.tag),
  loadOptions(other.loadOptions)
{
}

FileFormatInfo::FileFormatInfo(FileFormatInfo &&other) :
  longDescription(std::move(other.longDescription)),
  shortDescription(std::move(other.shortDescription)),
  tag(std::move(other.tag)),
  loadOptions(std::move(other.loadOptions))
{
}

FileFormatInfo & FileFormatInfo::operator=(const FileFormatInfo &other)
{
  const_cast<QString&>(longDescription) = other.longDescription;
  const_cast<QString&>(shortDescription) = other.shortDescription;
  const_cast<QString&>(tag) = other.tag;
  const_cast<QVector<QString>&>(loadOptions) = other.loadOptions;

  return *this;
}

Data::Data() :
  valid(false),
  path(""),
  name(""),
  xDescription(""),
  yDescription(""),
  xUnit(""),
  yUnit(""),
  datapoints(QVector<std::tuple<double, double>>())
{
}

Data::Data(const QString &path, const QString &name, const QString &xDescription, const QString &yDescription,
           const QString &xUnit, const QString &yUnit, const QVector<std::tuple<double, double>> &datapoints) :
  valid(true),
  path(path),
  name(name),
  xDescription(xDescription),
  yDescription(yDescription),
  xUnit(xUnit),
  yUnit(yUnit),
  datapoints(datapoints)
{
}

Data::Data(const QString &path, const QString &name, const QString &xDescription, const QString &yDescription,
           const QString &xUnit, const QString &yUnit, const QVector<std::tuple<double, double> > &&datapoints) :
  valid(true),
  path(path),
  name(name),
  xDescription(xDescription),
  yDescription(yDescription),
  xUnit(xUnit),
  yUnit(yUnit),
  datapoints(std::move(datapoints))
{
}

DataLoader::DataLoader(QObject *parent) :
  QObject(parent)
{
  if (!loadPlugins())
    throw std::runtime_error{"Cannot load backends"};
}

bool DataLoader::checkTag(const QString &tag) const
{
  return m_backendInstances.contains(tag);
}

void DataLoader::initializePlugin(const QString &pluginPath)
{
  if (!QLibrary::isLibrary(pluginPath)) {
    std::cerr << "File " << pluginPath.toStdString() << " is not a library\n";
    return;
  }

  QLibrary plugin(pluginPath);

  if (!plugin.load()) {
    std::cerr << "Could not load plugin" << std::endl;
    return;
  }

  backend::BackendInitializer initializer = (backend::BackendInitializer) plugin.resolve("initialize");
  if (initializer == nullptr) {
    std::cerr << "Could not resolve initializer symbol" << std::endl;
    return;
  }

  backend::LoaderBackend *instance = dynamic_cast<backend::LoaderBackend *>(initializer());
  if (instance == nullptr) {
    std::cerr << "Unable to get LoaderPlugin interface";
    return;
  }

  backend::Identifier ident = instance->identifier();

  std::cerr << ident.longDescription << ", " << ident.tag << std::endl;

  QString tag = QString::fromStdString(ident.tag);

  if (m_backendInstances.contains(tag))
    return;

  m_backendInstances.insert(tag, instance);
}



DataLoader::LoadedPack DataLoader::loadData(const QString &formatTag, const int mode) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->load(mode);

  if (pdVec.size() < 1)
    return makeErrorPack("No data was loaded");

  return package(pdVec);
}

std::tuple<std::vector<Data>, bool, QString> DataLoader::loadDataHint(const QString &formatTag, const QString &hintPath, const int mode) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->loadHint(hintPath.toStdString(), mode);

  if (pdVec.size() < 1)
    return makeErrorPack("No data was loaded");

  return package(pdVec);
}

std::tuple<std::vector<Data>, bool, QString> DataLoader::loadDataPath(const QString &formatTag, const QString &path, const int mode) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->loadPath(path.toStdString(), mode);

  if (pdVec.size() < 1)
    return makeErrorPack("No data was loaded");

  return package(pdVec);
}


bool DataLoader::loadPlugins()
{
  QDir dir = QDir::current();
  if (!dir.cd("efgbackends"))
    return false;

  QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

  for (const QString &s : files) {
    if (!s.endsWith(".so"))
      continue;

    QString symlinkPath = QFile(s).symLinkTarget();
    if (symlinkPath.length() > 0)
      initializePlugin(dir.filePath(symlinkPath));
    else
      initializePlugin(dir.filePath(s));
  }

  return true;
}

DataLoader::LoadedPack DataLoader::makeErrorPack(const QString &error) const
{
  return makePack(std::vector<Data>(), false, error);
}

DataLoader::LoadedPack DataLoader::makePack(const std::vector<Data> &data, const bool status, const QString &message) const
{
  return std::make_tuple(data, status, message);
}

DataLoader::LoadedPack DataLoader::package(std::vector<backend::Data> &vec) const
{
  std::vector<Data> packageVec;

  for (const auto &pd : vec) {
    QVector<std::tuple<double, double>> datapoints;
    for (const auto &item : pd.datapoints)
      datapoints.append(item);

    Data d{QString::fromStdString(pd.path),
           QString::fromStdString(pd.name),
           QString::fromStdString(pd.xDescription),
           QString::fromStdString(pd.yDescription),
           QString::fromStdString(pd.xUnit),
           QString::fromStdString(pd.yUnit),
           datapoints};

    packageVec.emplace_back(d);
  }

  return makePack(packageVec, true);
}

QVector<FileFormatInfo> DataLoader::supportedFileFormats() const
{
  QVector<FileFormatInfo> vec;

  for (const auto &item : m_backendInstances) {
    backend::Identifier ident = item->identifier();
    QVector<QString> loadOptions;

    for (const std::string &s : ident.loadOptions)
      loadOptions.push_back(QString::fromStdString(s));

    vec.push_back(FileFormatInfo{QString::fromStdString(ident.longDescription),
                                 QString::fromStdString(ident.shortDescription),
                                 QString::fromStdString(ident.tag),
                                 loadOptions});
  }

  return vec;
}
