#ifndef CHEMSTATIONBATCHLOADER_H
#define CHEMSTATIONBATCHLOADER_H

#include <QDir>
#include "chemstationfileloader.h"

class ChemStationBatchLoader
{
public:
  class KeyFileData {
  public:
    explicit KeyFileData(const ChemStationFileLoader::Type type = ChemStationFileLoader::Type::CE_UNKNOWN, const int wlMeasured = 0, const int wlReference = 0);

    const ChemStationFileLoader::Type type;
    const int wlMeasured;
    const int wlReference;

    KeyFileData & operator=(const KeyFileData &other);
    bool operator==(const KeyFileData &other) const;
  };

  typedef QVector<KeyFileData> KeyFileDataVec;
  typedef QVector<KeyFileDataVec> KeyFileDataVecVec;

  ChemStationBatchLoader() = delete;

  static KeyFileDataVec inspectDirectory(const QString &path);
  static KeyFileDataVec inspectDirectories(const QStringList &dirPaths);

private:
  static KeyFileDataVec getChemStationFiles(const QDir &dir);
  static KeyFileDataVec intersection(const KeyFileDataVecVec &kfvVec);
};

#endif // CHEMSTATIONBATCHLOADER_H
