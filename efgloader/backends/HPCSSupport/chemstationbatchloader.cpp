#include "chemstationbatchloader.h"
#include <QDirIterator>
#include <QString>

bool operator==(const ChemStationFileLoader::Data &d1, const ChemStationFileLoader::Data &d2)
{
  if (d1.type != d2.type)
    return false;

  if (d1.type == ChemStationFileLoader::Type::CE_DAD) {
    return (d1.wavelengthMeasured.wavelength == d2.wavelengthMeasured.wavelength) &&
           (d1.wavelengthReference.wavelength == d2.wavelengthReference.wavelength);
  }

  return true;
}

ChemStationBatchLoader::Filter::Filter() :
  type(ChemStationFileLoader::Type::CE_UNKNOWN),
  wlMeasured(0),
  wlReference(0),
  isValid(false)
{
}

ChemStationBatchLoader::Filter::Filter(const ChemStationFileLoader::Type type, const int wlMeasured, const int wlReference) :
  type(type),
  wlMeasured(wlMeasured),
  wlReference(wlReference),
  isValid(true)
{
}

ChemStationBatchLoader::Filter::Filter(const Filter &other) :
  type(other.type),
  wlMeasured(other.wlMeasured),
  wlReference(other.wlReference),
  isValid(other.isValid)
{
}

ChemStationBatchLoader::Filter & ChemStationBatchLoader::Filter::operator=(const Filter &other)
{
  const_cast<ChemStationFileLoader::Type&>(type) = other.type;
  const_cast<int&>(wlMeasured) = other.wlMeasured;
  const_cast<int&>(wlReference) = other.wlReference;
  const_cast<bool&>(isValid) = other.isValid;

  return *this;
}

bool ChemStationBatchLoader::filterMatches(const ChemStationFileLoader::Data &chData, const Filter &filter)
{
  if (chData.type != filter.type)
    return false;

  if (chData.type == ChemStationFileLoader::Type::CE_DAD) {
    return (chData.wavelengthMeasured.wavelength == filter.wlMeasured) &&
           (chData.wavelengthReference.wavelength == filter.wlReference);
  }

  return true;
}

ChemStationBatchLoader::CHSDataVec ChemStationBatchLoader::getChemStationFiles(UIBackend *backend, const QDir &dir)
{
  CHSDataVec chsVec;

  QDirIterator dirIt(dir.absolutePath(), QDir::Files | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    const ChemStationFileLoader::Data chData = ChemStationFileLoader::loadHeader(backend, dirIt.next());

    if (!chData.isValid())
      continue;

    chsVec.push_back(chData);
  }

  return chsVec;
}

QStringList ChemStationBatchLoader::getFilesList(UIBackend *backend, const QString &path, const Filter &filter)
{
  QDir dir(path);
  QStringList files;

  if (!dir.exists() || !dir.isReadable())
    return files;

  QDirIterator dirIt(path, QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext())
    files.append(walkDirectory(backend, dirIt.next(), filter));

  return files;
}

QStringList ChemStationBatchLoader::getFilesList(UIBackend *backend, const QStringList &dirPaths, const Filter &filter)
{
  QStringList files;

  for (const QString &path : dirPaths) {
    QDir dir(path);

    if (!dir.exists() || !dir.isReadable())
      continue;

    files.append(walkDirectory(backend, path, filter));
  }

  return files;
}

ChemStationBatchLoader::CHSDataVec ChemStationBatchLoader::getCommonTypes(UIBackend *backend, const QString &path)
{
  QDir dir(path);
  CHSDataVecVec chVecVec;
  CHSDataVec common;

  if (!dir.exists() || !dir.isReadable())
    return common;

  QDirIterator dirIt(path, QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    QDir innerDir(dirIt.next());

    if (innerDir.isReadable())
      chVecVec.push_back(getChemStationFiles(backend, innerDir));

  }

  if (chVecVec.size() > 0)
    common = intersection(chVecVec);

  return common;
}

ChemStationBatchLoader::CHSDataVec ChemStationBatchLoader::getCommonTypes(UIBackend *backend, const QStringList &dirPaths)
{
  CHSDataVecVec chVecVec;
  CHSDataVec common;

  for (const QString &path : dirPaths) {
    QDir dir(path);

    if (!dir.exists() || !dir.isReadable())
      continue;

    chVecVec.push_back(getChemStationFiles(backend, dir));
  }

  if (chVecVec.size() > 0)
    common = intersection(chVecVec);

  return common;
}

ChemStationBatchLoader::CHSDataVec ChemStationBatchLoader::intersection(const CHSDataVecVec &chvVec)
{
  CHSDataVec common;
  const CHSDataVec &first = chvVec.front();

  if (chvVec.size() < 2)
    return first;

  for (const ChemStationFileLoader::Data &chDataFirst : first) {
    bool isCommon = true;

    for (int idx = 1; idx <  chvVec.size(); idx++) {
      const CHSDataVec &next = chvVec.at(idx);

      int jdx;
      for (jdx = 0; jdx < next.size(); jdx++) {
        const ChemStationFileLoader::Data &kfDataNext = next.at(jdx);

        if (chDataFirst == kfDataNext)
          break;
      }

      if (jdx == next.size()) {
        isCommon = false;
        break;
      }
    }

    if (isCommon)
      common.push_back(chDataFirst);
  }

  return common;
}

QStringList ChemStationBatchLoader::walkDirectory(UIBackend *backend, const QString &path, const Filter &filter)
{
  QStringList files;
  QDirIterator dirIt(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    QString filePath = dirIt.next();

    ChemStationFileLoader::Data chData = ChemStationFileLoader::loadHeader(backend, filePath);

    if (!chData.isValid())
      continue;

    if (filterMatches(chData, filter))
      files.push_back(filePath);
  }

  return files;
}
