#include "loadchemstationdatadialog.h"
#include "ui_loadchemstationdatadialog.h"
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include "../chemstationbatchloader.h"

#include <QDebug>

LoadChemStationDataDialog::LoadChemStationDataDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LoadChemStationDataDialog)
{
  ui->setupUi(this);

  try {
    m_fsModel = new QFileSystemModel(this);
    m_finfoModel = new ChemStationFileInfoModel(this);
    qs_splitter = new QSplitter(Qt::Horizontal, this);
    qtrv_fileSystem = new QTreeView(qs_splitter);
    qtbv_files = new QTableView(qs_splitter);
  } catch (std::bad_alloc&) {
    return;
  }

  m_fsModel->setReadOnly(true);
  m_fsModel->setResolveSymlinks(false);
  m_fsModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);


  qtrv_fileSystem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  qtbv_files->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  qtrv_fileSystem->setModel(m_fsModel);
  qtbv_files->setModel(m_finfoModel);

  ui->qcbox_loadingMode->addItem(tr("Single file"), QVariant::fromValue(LoadingMode::SINGLE_FILE));
  ui->qcbox_loadingMode->addItem(tr("Whole directory"), QVariant::fromValue(LoadingMode::WHOLE_DIRECTORY));
  ui->qcbox_loadingMode->addItem(tr("Multiple directories"), QVariant::fromValue(LoadingMode::MULTIPLE_DIRECTORIES));
  m_loadingMode = LoadingMode::SINGLE_FILE;

  /* Hide all additonal information and show only the name
   * of the file in the browsing tree */
  qtrv_fileSystem->hideColumn(1);
  qtrv_fileSystem->hideColumn(2);
  qtrv_fileSystem->hideColumn(3);

  qtbv_files->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  ui->panesLayout->addWidget(qs_splitter);
  qs_splitter->addWidget(qtrv_fileSystem);
  qs_splitter->addWidget(qtbv_files);

  connect(qtrv_fileSystem, &QTreeView::clicked, this, &LoadChemStationDataDialog::onClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &LoadChemStationDataDialog::onCancelClicked);
  connect(ui->qpb_load, &QPushButton::clicked, this, &LoadChemStationDataDialog::onLoadClicked);
  connect(qtbv_files, &QTableView::doubleClicked, this, &LoadChemStationDataDialog::onFilesDoubleClicked);
  connect(ui->qcbox_loadingMode, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &LoadChemStationDataDialog::onLoadingModeActivated);
}

LoadChemStationDataDialog::~LoadChemStationDataDialog()
{
  delete ui;
}

QString LoadChemStationDataDialog::createFileType(const ChemStationFileLoader::Type type)
{
  switch (type) {
  case ChemStationFileLoader::Type::CE_ANALOG:
    return "Analog input";
    break;
  case ChemStationFileLoader::Type::CE_CCD:
    return "Conductivity";
    break;
  case ChemStationFileLoader::Type::CE_CURRENT:
    return "Current";
    break;
  case ChemStationFileLoader::Type::CE_DAD:
    return "Absorbance";
    break;
  case ChemStationFileLoader::Type::CE_POWER:
    return "Power";
    break;
  case ChemStationFileLoader::Type::CE_PRESSURE:
    return "Pressure";
    break;
  case ChemStationFileLoader::Type::CE_TEMPERATURE:
    return "Temperature";
    break;
  case ChemStationFileLoader::Type::CE_VOLTAGE:
    return "Voltage";
    break;
  case ChemStationFileLoader::Type::CE_UNKNOWN:
  default:
    return "Unknown";
    break;
  }
}

QString LoadChemStationDataDialog::createAdditionalInfo(const ChemStationFileLoader::Data &data)
{
  if (data.type == ChemStationFileLoader::Type::CE_DAD) {
    return QString("Measured: %1 nm / Reference %2 nm").arg(data.wavelengthMeasured.wavelength).arg(data.wavelengthReference.wavelength);
  }
  return QString();
}

void LoadChemStationDataDialog::expandToPath(const QString &path)
{
  const QModelIndex &index = m_fsModel->index(path);

  if (!index.isValid())
    return;

  m_fsModel->setRootPath(path);

  qtrv_fileSystem->setCurrentIndex(index);
  qtrv_fileSystem->scrollTo(index);
  onClicked(index);
}

QString LoadChemStationDataDialog::lastSelectedFile()
{
  return m_lastSelectedFile;
}

void LoadChemStationDataDialog::multipleDirectoriesSelected()
{
  const QModelIndexList &indexes = qtrv_fileSystem->selectionModel()->selectedIndexes();
  QStringList dirPaths;

  for (const QModelIndex &index : indexes) {
    dirPaths.push_back(m_fsModel->filePath(index));
  }

  ChemStationBatchLoader::KeyFileDataVec common = ChemStationBatchLoader::inspectDirectories(dirPaths);

  /* TESTING */
  for (const ChemStationBatchLoader::KeyFileData &kfData : common) {
    qDebug() << (int)kfData.type << kfData.wlMeasured << kfData.wlReference;
  }
  qDebug("---\n");
}

void LoadChemStationDataDialog::onCancelClicked(const bool clicked)
{
  Q_UNUSED(clicked);

  reject();
}

void LoadChemStationDataDialog::onClicked(const QModelIndex &index)
{
  if (!index.isValid()) {
    m_finfoModel->clear();
    return;
  }

  switch (m_loadingMode) {
  case LoadingMode::SINGLE_FILE:
    singleSelected(index);
    break;
  case LoadingMode::WHOLE_DIRECTORY:
    wholeDirectorySelected(index);
    break;
  case LoadingMode::MULTIPLE_DIRECTORIES:
    multipleDirectoriesSelected();
    break;
  default:
    break;
  }
}

void LoadChemStationDataDialog::onFilesDoubleClicked(const QModelIndex &index)
{
  QVariant var;

  if (!index.isValid())
    return;

  var = m_finfoModel->data(m_finfoModel->index(index.row(), 0), Qt::DisplayRole);

  if (!processFileName(var))
    return;
  accept();

}

void LoadChemStationDataDialog::onLoadClicked(const bool clicked)
{
  Q_UNUSED(clicked);
  QModelIndex idx;
  QVariant var;

  idx = m_finfoModel->index(qtbv_files->currentIndex().row(), 0);
  var = m_finfoModel->data(idx, Qt::DisplayRole);

  if (!processFileName(var))
    return;
  accept();
}

void LoadChemStationDataDialog::onLoadingModeActivated()
{
  QVariant v = ui->qcbox_loadingMode->currentData(Qt::UserRole);

  if (!v.canConvert<LoadingMode>())
    return;

  m_loadingMode = v.value<LoadingMode>();

  switch (m_loadingMode) {
  case LoadingMode::SINGLE_FILE:
  case LoadingMode::WHOLE_DIRECTORY:
    qtrv_fileSystem->setSelectionMode(QAbstractItemView::SingleSelection);
    qtrv_fileSystem->clearSelection();
    m_finfoModel->clear();
    break;
  case LoadingMode::MULTIPLE_DIRECTORIES:
    qtrv_fileSystem->setSelectionMode(QAbstractItemView::ExtendedSelection);
    break;
  }
}

bool LoadChemStationDataDialog::processFileName(const QVariant &fileNameVariant)
{
  QDir dir(m_currentDirPath);

  if (!fileNameVariant.isValid())
    return false;

  m_lastSelectedFile = dir.absoluteFilePath(fileNameVariant.toString());
  return true;
}

void LoadChemStationDataDialog::singleSelected(const QModelIndex &index)
{
  QVector<ChemStationFileInfoModel::Entry> entries;

  m_currentDirPath = m_fsModel->filePath(index);
  QDir dir(m_currentDirPath);

  if (!dir.exists())
    return;

  QDirIterator dirIt(m_currentDirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);

  while (dirIt.hasNext()) {
    QString fileName = dirIt.next();
    QString absoluteFilePath = dir.absoluteFilePath(fileName);
    QString name = QFileInfo(absoluteFilePath).fileName();

    ChemStationFileLoader::Data data = ChemStationFileLoader::loadHeader(absoluteFilePath);
    if (!data.isValid())
      entries.push_back(ChemStationFileInfoModel::Entry(name, "", "", false));
    else
      entries.push_back(ChemStationFileInfoModel::Entry(name, createFileType(data.type), createAdditionalInfo(data), true));
  }


  m_finfoModel->setNewData(entries);
  m_finfoModel->sort(0, Qt::AscendingOrder);
}

void LoadChemStationDataDialog::wholeDirectorySelected(const QModelIndex &index)
{
  ChemStationBatchLoader::KeyFileDataVec common = ChemStationBatchLoader::inspectDirectory(m_fsModel->filePath(index));

  /* TESTING */
  for (const ChemStationBatchLoader::KeyFileData &kfData : common) {
    qDebug() << (int)kfData.type << kfData.wlMeasured << kfData.wlReference;
  }
  qDebug("---\n");
}
