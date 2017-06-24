#include "dataloader.h"
#include "common/backendinterface.h"
#include <QDir>
#include <QLibrary>

#include <iostream>

FileFormatInfo::FileFormatInfo() :
  description(""),
  tag("")
{
}

FileFormatInfo::FileFormatInfo(const QString &description, const QString &tag) :
  description(description),
  tag(tag)
{
}

FileFormatInfo::FileFormatInfo(const FileFormatInfo &other) :
  description(other.description),
  tag(other.tag)
{
}

FileFormatInfo::FileFormatInfo(FileFormatInfo &&other) :
  description(std::move(other.description)),
  tag(std::move(other.tag))
{
}

FileFormatInfo & FileFormatInfo::operator=(const FileFormatInfo &other)
{
  const_cast<QString&>(description) = other.description;
  const_cast<QString&>(tag) = other.tag;

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

  std::cerr << ident.description << ", " << ident.tag << std::endl;

  QString tag = QString::fromStdString(ident.tag);

  if (m_backendInstances.contains(tag))
    return;

  m_backendInstances.insert(tag, instance);
}



DataLoader::LoadedPack DataLoader::loadData(const QString &formatTag) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->load();

  if (pdVec.size() < 1)
    return makeErrorPack("No data was loaded");

  return package(pdVec);
}

std::tuple<std::vector<Data>, bool, QString> DataLoader::loadDataHint(const QString &formatTag, const QString &hintPath) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->loadHint(hintPath.toStdString());

  if (pdVec.size() < 1)
    return makeErrorPack("No data was loaded");

  return package(pdVec);
}

std::tuple<std::vector<Data>, bool, QString> DataLoader::loadDataPath(const QString &formatTag, const QString &path) const
{
  if (!checkTag(formatTag))
    return makeErrorPack(QString("Invalid format tag %1").arg(formatTag));

  backend::LoaderBackend *instance = m_backendInstances[formatTag];
  std::vector<backend::Data> pdVec = instance->loadPath(path.toStdString());

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
    vec.append(FileFormatInfo{QString::fromStdString(ident.description), QString::fromStdString(ident.tag)});
  }

  return vec;
}
