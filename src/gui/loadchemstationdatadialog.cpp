#include "loadchemstationdatadialog.h"
#include "ui_loadchemstationdatadialog.h"
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>

LoadChemStationDataDialog::LoadChemStationDataDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LoadChemStationDataDialog)
{
  ui->setupUi(this);

  try {
    m_fsModel = new QDirModel(this);
    m_finfoModel = new ChemStationFileInfoModel(this);
    qs_splitter = new QSplitter(Qt::Horizontal, this);
    qtrv_fileSystem = new QTreeView(qs_splitter);
    qtbv_files = new QTableView(qs_splitter);
  } catch (std::bad_alloc& ) {
    return;
  }

  m_fsModel->setReadOnly(true);
  m_fsModel->setResolveSymlinks(false);
  m_fsModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
  m_fsModel->setSorting(QDir::Name | QDir::IgnoreCase);


  qtrv_fileSystem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  qtbv_files->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  qtrv_fileSystem->setModel(m_fsModel);
  qtbv_files->setModel(m_finfoModel);

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
  qtrv_fileSystem->setCurrentIndex(index);
  qtrv_fileSystem->scrollTo(index);
  onClicked(index);
}

QString LoadChemStationDataDialog::lastSelectedFile()
{
  return m_lastSelectedFile;
}

void LoadChemStationDataDialog::onCancelClicked(const bool clicked)
{
  Q_UNUSED(clicked);

  reject();
}

void LoadChemStationDataDialog::onClicked(const QModelIndex &index)
{
  QVector<ChemStationFileInfoModel::Entry> entries;

  if (!index.isValid())
    return;

  m_fsModel->refresh(index);

  m_currentDirPath = m_fsModel->data(index, QDirModel::FilePathRole).toString();
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

bool LoadChemStationDataDialog::processFileName(const QVariant &fileNameVariant)
{
  QDir dir(m_currentDirPath);

  if (!fileNameVariant.isValid())
    return false;

  m_lastSelectedFile = dir.absoluteFilePath(fileNameVariant.toString());
  return true;
}


void LoadChemStationDataDialog::refresh()
{
  if (QDir(m_currentDirPath).exists())
    m_fsModel->refresh();
}
