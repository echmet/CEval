#ifndef HPCSSUPPORT_H
#define HPCSSUPPORT_H

#include "hpcssupport_global.h"
#include "../../core/common/backendinterface.h"
#include <QStringList>
#include <QSize>
#include "chemstationbatchloader.h"

class LoadChemStationDataDialog;

namespace backend {

class HPCSSUPPORTSHARED_EXPORT HPCSSupport : public LoaderBackend
{
public:
  virtual Identifier identifier() const override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static HPCSSupport *instance();

private:
  HPCSSupport();
  ~HPCSSupport();
  std::string chemStationTypeToString(const ChemStationFileLoader::Type type);
  QString defaultPath() const;
  bool isDirectoryUsable(const QString &path) const;
  Data loadChemStationFileSingle(const QString &path);
  void loadChemStationFileMultipleDirectories(std::vector<Data> &dataVec, const QStringList &dirPaths, const ChemStationBatchLoader::Filter &filter);
  void loadChemStationFileWholeDirectory(std::vector<Data> &dataVec, const QString &path, const ChemStationBatchLoader::Filter &filter);

  QStringList m_defaultPathsToTry;
  LoadChemStationDataDialog *m_loadChemStationDataDlg;
  QSize m_lastChemStationDlgSize;
  QString m_lastChemStationPath;

  static HPCSSupport *s_me;
  static Identifier s_identifier;
};

extern "C" {
  HPCSSUPPORTSHARED_EXPORT LoaderBackend * initialize();
}

} // namespace backend

#endif // HPCSSUPPORT_H
