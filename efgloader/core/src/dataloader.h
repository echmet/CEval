#ifndef DATALOADER_H
#define DATALOADER_H

#include <QMap>
#include <QObject>
#include <QVector>
#include "common/backendinterface.h"

class FileFormatInfo {
public:
  explicit FileFormatInfo();
  explicit FileFormatInfo(const QString &longDescription, const QString &shortDescription, const QString &tag);
  FileFormatInfo(const FileFormatInfo &other);
  FileFormatInfo(FileFormatInfo &&other);

  FileFormatInfo & operator=(const FileFormatInfo &other);

  const QString longDescription;
  const QString shortDescription;
  const QString tag;
};

class Data {
public:
  explicit Data();
  explicit Data(const QString &path, const QString &name,
                const QString &xDescription, const QString &yDescription,
                const QString &xUnit, const QString &yUnit,
                const QVector<std::tuple<double, double>> &datapoints);
  explicit Data(const QString &path, const QString &name,
                const QString &xDescription, const QString &yDescription,
                const QString &xUnit, const QString &yUnit,
                const QVector<std::tuple<double, double>> &&datapoints);

  const bool valid;
  const QString path;
  const QString name;
  const QString xDescription;
  const QString yDescription;
  const QString xUnit;
  const QString yUnit;
  const QVector<std::tuple<double, double>> datapoints;
};

class DataLoader : public QObject
{
  Q_OBJECT

public:
  typedef std::tuple<std::vector<Data>, bool ,QString> LoadedPack;

  explicit DataLoader(QObject *parent = nullptr);
  std::tuple<std::vector<Data>, bool, QString> loadData(const QString &formatTag) const;
  std::tuple<std::vector<Data>, bool, QString> loadDataHint(const QString &formatTag, const QString &hintPath) const;
  std::tuple<std::vector<Data>, bool, QString> loadDataPath(const QString &formatTag, const QString &path) const;
  QVector<FileFormatInfo> supportedFileFormats() const;

private:
  bool checkTag(const QString &tag) const;
  void initializePlugin(const QString &pluginPath);
  bool loadPlugins();
  LoadedPack makeErrorPack(const QString &error) const;
  LoadedPack makePack(const std::vector<Data> &data, const bool status, const QString &message = "") const;
  LoadedPack package(std::vector<backend::Data> &vec) const;

  QMap<QString, backend::LoaderBackend *> m_backendInstances;

};

#endif // DATALOADER_H
