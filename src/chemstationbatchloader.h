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
    Filter(const Filter &other);
    Filter & operator=(const Filter &other);

    const ChemStationFileLoader::Type type;
    const int wlMeasured;
    const int wlReference;
    const bool isValid;
  };

  typedef QVector<ChemStationFileLoader::Data> CHSDataVec;
  typedef QVector<CHSDataVec> CHSDataVecVec;

  ChemStationBatchLoader() = delete;

  static bool filterMatches(const ChemStationFileLoader::Data &chData, const Filter &filter);
  static CHSDataVec getCommonTypes(const QString &path);
  static CHSDataVec getCommonTypes(const QStringList &dirPaths);
  static QStringList getFilesList(const QString &path, const Filter &filter);
  static QStringList getFilesList(const QStringList &dirPaths, const Filter &filter);

private:
  static CHSDataVec getChemStationFiles(const QDir &dir);
  static CHSDataVec intersection(const CHSDataVecVec &chvVec);
  static QStringList walkDirectory(const QString &path, const Filter &filter);

};
//Q_DECLARE_METATYPE(ChemStationBatchLoader::Filter)

#endif // CHEMSTATIONBATCHLOADER_H
