#ifndef LOADCHEMSTATIONDATADIALOG_H
#define LOADCHEMSTATIONDATADIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTableView>
#include <QSplitter>
#include "../chemstationfileinfomodel.h"
#include "../chemstationfileloader.h"
#include "../chemstationbatchloader.h"

class ChemStationBatchLoadModel;

namespace Ui {
class LoadChemStationDataDialog;
}

class LoadChemStationDataDialog : public QDialog
{
  Q_OBJECT
public:
  enum class LoadingMode {
    SINGLE_FILE,
    WHOLE_DIRECTORY,
    MULTIPLE_DIRECTORIES
  };
  Q_ENUM(LoadingMode);

  class LoadInfo {
  public:
    explicit LoadInfo(const LoadingMode loadingMode, const QString &path,
                      const QStringList &dirPaths = QStringList(),
                      const ChemStationBatchLoader::Filter &filter = ChemStationBatchLoader::Filter());
    LoadInfo(const LoadInfo &other);
    LoadInfo & operator=(const LoadInfo &other);

    const LoadingMode loadingMode;
    const QString path;
    const QStringList dirPaths;

    /* Used only in WHOLE_DIRECTORY or MULTIPLE_DIRECTORIES mode */
    const ChemStationBatchLoader::Filter filter;
  };

  explicit LoadChemStationDataDialog(QWidget *parent = nullptr);
  ~LoadChemStationDataDialog();
  void expandToPath(const QString &path);
  LoadInfo loadInfo();

private:
  QString createFileType(const ChemStationFileLoader::Type type);
  QString createAdditionalInfo(const ChemStationFileLoader::Data &data);
  void loadSingleFile(const QModelIndex &index);
  void loadMultipleDirectories(const QModelIndex &index);
  void loadWholeDirectory(const QModelIndex &index);
  void multipleDirectoriesSelected();
  QString processFileName(const QVariant &fileNameVariant, bool &ok);
  void singleSelected(const QModelIndex &index);
  void wholeDirectorySelected(const QModelIndex &index);

  Ui::LoadChemStationDataDialog *ui;
  QSplitter *qs_splitter;
  QTreeView *qtrv_fileSystem;
  QTableView *qtbv_files;
  QString m_currentDirPath;
  QFileSystemModel *m_fsModel;
  ChemStationFileInfoModel *m_finfoModel;
  ChemStationBatchLoadModel *m_batchLoadModel;
  QString m_lastSelectedFile;
  LoadingMode m_loadingMode;
  LoadInfo m_loadInfo;


signals:
  void loadFile(const QString &path);

private slots:
  void onCancelClicked(const bool clicked);
  void onClicked(const QModelIndex &index);
  void onFilesDoubleClicked(const QModelIndex &index);
  void onLoadClicked(const bool clicked);
  void onLoadingModeActivated();

};

#endif // LOADCHEMSTATIONDATADIALOG_H
