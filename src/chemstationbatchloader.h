#ifndef CHEMSTATIONBATCHLOADER_H
#define CHEMSTATIONBATCHLOADER_H

#include <QDir>
#include "chemstationfileloader.h"

class ChemStationBatchLoader
{
public:
  class Filter {
  public:
    explicit Filter();
    explicit Filter(const ChemStationFileLoader::Type type, const int wlMeasured, const int wlReference);

    ChemStationFileLoader::Type type;
    int wlMeasured;
    int wlReference;
    bool isValid;
  };

  typedef QVector<ChemStationFileLoader::Data> CHSDataVec;
  typedef QVector<CHSDataVec> CHSDataVecVec;

  ChemStationBatchLoader() = delete;

  static bool filterMatches(const ChemStationFileLoader::Data &chData, const Filter &filter);
  static QStringList getFilesList(const QString &path, const Filter &filter);
  static QStringList getFilesList(const QStringList &dirPaths, const Filter &filter);
  static CHSDataVec inspectDirectory(const QString &path);
  static CHSDataVec inspectDirectories(const QStringList &dirPaths);

private:
  static CHSDataVec getChemStationFiles(const QDir &dir);
  static CHSDataVec intersection(const CHSDataVecVec &kfvVec);
  static QStringList walkDirectory(const QString &path, const Filter &filter);

};
Q_DECLARE_METATYPE(ChemStationBatchLoader::Filter)

#endif // CHEMSTATIONBATCHLOADER_H
