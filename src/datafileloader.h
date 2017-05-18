#ifndef DATAFILELOADER_H
#define DATAFILELOADER_H

#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QVector>
#include <QPointF>
#include <memory>
#include "chemstationfileloader.h"
#include "csvfileloader.h"
#include "datafileloadermsgs.h"
#include "chemstationbatchloader.h"
#include "gui/loadcsvfiledialog.h"

class LoadChemStationDataDialog;

class DataFileLoader : public QObject
{
  Q_OBJECT
public:
  class Data {
  public:
    explicit Data(const QVector<QPointF> data, const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit);
    explicit Data();
    explicit Data(const Data &other);
    bool isValid() const;
    Data &operator=(const Data &other);

    const QVector<QPointF> data;
    QString xType;
    QString xUnit;
    QString yType;
    QString yUnit;
  private:
     const bool m_valid;
  };

  explicit DataFileLoader(QObject *parent = nullptr);
  ~DataFileLoader();
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings() const;

private:
  QString chemStationTypeToString(const ChemStationFileLoader::Type type);
  QString defaultPath() const;
  bool isDirectoryUsable(const QString &path) const;
  void loadChemStationFile();
  void loadChemStationFileSingle(const QString &path);
  void loadChemStationFileMultipleDirectories(const QStringList &dirFiles, const ChemStationBatchLoader::Filter &filter);
  void loadChemStationFileWholeDirectory(const QString &path, const ChemStationBatchLoader::Filter &filter);
  void loadCsvData(const CsvFileLoader::Data &csvData, const QString &file, const LoadCsvFileDialog::Parameters &p);
  void loadCsvFromFile();
  void loadCsvFromClipboard();
  void loadNetCDFFile();
  CsvFileLoader::Parameters makeCsvLoaderParameters();

  LoadChemStationDataDialog *m_loadChemStationDataDlg;
  LoadCsvFileDialog *m_loadCsvFileDlg;

  QString m_lastChemStationPath;
  QString m_lastCsvPath;
  QString m_lastNetCDFPath;
  QSize m_lastChemStationDlgSize;

  const QStringList m_defaultPathsToTry;

  static const QString LAST_CHEMSTATION_LOAD_PATH_SETTINGS_TAG;
  static const QString LAST_CSV_LOAD_PATH_SETTINGS_TAG;
  static const QString LAST_NETCDF_LOAD_PATH_SETTINGS_TAG;
  static const QString LAST_CHEMSTATION_DLG_SIZE_TAG;

signals:
  void dataLoaded(std::shared_ptr<Data> data, const QString &fullPath, const QString &fileName);

public slots:
  void onLoadDataFile(const DataFileLoaderMsgs::LoadableFileTypes type);
};

#endif // DATAFILELOADER_H
