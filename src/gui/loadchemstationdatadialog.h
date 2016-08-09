#ifndef LOADCHEMSTATIONDATADIALOG_H
#define LOADCHEMSTATIONDATADIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTableView>
#include <QSplitter>
#include "../chemstationfileloader.h"
#include "../chemstationfileinfomodel.h"

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

  explicit LoadChemStationDataDialog(QWidget *parent = nullptr);
  ~LoadChemStationDataDialog();
  void expandToPath(const QString &path);
  QString lastSelectedFile();

private:
  QString createFileType(const ChemStationFileLoader::Type type);
  QString createAdditionalInfo(const ChemStationFileLoader::Data &data);
  Ui::LoadChemStationDataDialog *ui;
  QSplitter *qs_splitter;
  QTreeView *qtrv_fileSystem;
  QTableView *qtbv_files;
  QString m_currentDirPath;
  QFileSystemModel *m_fsModel;
  ChemStationFileInfoModel *m_finfoModel;
  QString m_lastSelectedFile;
  LoadingMode m_loadingMode;

private:
  bool processFileName(const QVariant &fileNameVariant);

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
