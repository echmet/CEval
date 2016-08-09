#include "chemstationbatchloader.h"
#include <QDirIterator>
#include <QString>

#include <QDebug>

ChemStationBatchLoader::KeyFileData::KeyFileData(const ChemStationFileLoader::Type type, const int wlMeasured, const int wlReference) :
  type(type),
  wlMeasured(wlMeasured),
  wlReference(wlReference)
{
}

ChemStationBatchLoader::KeyFileData & ChemStationBatchLoader::KeyFileData::operator=(const KeyFileData &other)
{
  const_cast<ChemStationFileLoader::Type&>(type) = other.type;
  const_cast<int&>(wlMeasured) = other.wlMeasured;
  const_cast<int&>(wlReference) = other.wlReference;

  return *this;
}

bool ChemStationBatchLoader::KeyFileData::operator==(const KeyFileData &other) const
{
  return (type == other.type) &&
         (wlMeasured == other.wlMeasured) &&
         (wlReference == other.wlReference);
}

ChemStationBatchLoader::KeyFileDataVec ChemStationBatchLoader::getChemStationFiles(const QDir &dir)
{
  KeyFileDataVec kfVec;

  QDirIterator dirIt(dir.absolutePath(), QDir::Files | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    ChemStationFileLoader::Data chData = ChemStationFileLoader::loadHeader(dirIt.next());

    if (!chData.isValid())
      continue;

    int wlMeasured;
    int wlReference;

    if (chData.type == ChemStationFileLoader::Type::CE_DAD) {
      wlMeasured = chData.wavelengthMeasured.wavelength;
      wlReference = chData.wavelengthReference.wavelength;
    } else {
      wlMeasured = 0;
      wlReference = 0;
    }

    kfVec.push_back(KeyFileData(chData.type, wlMeasured, wlReference));
  }

  return kfVec;
}

ChemStationBatchLoader::KeyFileDataVec ChemStationBatchLoader::inspectDirectory(const QString &path)
{
  QDir dir(path);
  KeyFileDataVecVec kfVecVec;
  KeyFileDataVec common;

  if (!dir.exists() || !dir.isReadable())
    return common;

  QDirIterator dirIt(path, QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    QDir innerDir(dirIt.next());

    if (innerDir.isReadable()) {
      kfVecVec.push_back(getChemStationFiles(innerDir));
    }
  }

  if (kfVecVec.size() > 0)
    common = intersection(kfVecVec);

  return common;
}

ChemStationBatchLoader::KeyFileDataVec ChemStationBatchLoader::inspectDirectories(const QStringList &dirPaths)
{
  KeyFileDataVecVec kfVecVec;
  KeyFileDataVec common;

  for (const QString &path : dirPaths) {
    QDir dir(path);

    if (!dir.exists() || !dir.isReadable())
      continue;

    kfVecVec.push_back(getChemStationFiles(dir));
  }

  if (kfVecVec.size() > 0)
    common = intersection(kfVecVec);

  return common;
}

ChemStationBatchLoader::KeyFileDataVec ChemStationBatchLoader::intersection(const KeyFileDataVecVec &kfvVec)
{
  KeyFileDataVec common;
  const KeyFileDataVec &first = kfvVec.front();

  if (kfvVec.size() < 2)
    return first;

  for (const KeyFileData &kfDataFirst : first) {
    bool isCommon = true;

    for (int idx = 1; idx <  kfvVec.size(); idx++) {
      const KeyFileDataVec &next = kfvVec.at(idx);

      int jdx;
      for (jdx = 0; jdx < next.size(); jdx++) {
        const KeyFileData &kfDataNext = next.at(jdx);

        if (kfDataFirst == kfDataNext)
          break;
      }

      if (jdx == next.size()) {
        isCommon = false;
        break;
      }
    }

    if (isCommon)
      common.push_back(kfDataFirst);
  }

  return common;
}
