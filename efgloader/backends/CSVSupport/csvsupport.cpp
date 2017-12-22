#include "csvsupport.h"
#include <QFileDialog>
#include <QMessageBox>
#include "csvfileloader.h"
#include "loadcsvfiledialog.h"
#include "../../efgloader-core/common/backendhelpers_p.h"

namespace backend {

CSVSupport *CSVSupport::s_me{nullptr};
Identifier CSVSupport::s_identifier{"Comma separated file format support", "CSV", "CSV", {"file", "clipboard"}};

Data loadCsvData(const CsvFileLoader::Data &csvData, const QString &file, const LoadCsvFileDialog::Parameters &p)
{
  QString xType;
  QString yType;
  QString xUnit;
  QString yUnit;
  QString fileName;

  switch (p.header) {
  case LoadCsvFileDialog::HeaderHandling::NO_HEADER:
    xType = p.xType;
    yType = p.yType;
    xUnit = p.xUnit;
    yUnit = p.yUnit;
    break;
  case LoadCsvFileDialog::HeaderHandling::HEADER_WITH_UNITS:
    xType = csvData.xType;
    yType = csvData.yType;
    break;
  case LoadCsvFileDialog::HeaderHandling::HEADER_WITHOUT_UNITS:
    xType = csvData.xType;
    yType = csvData.yType;
    xUnit = p.xUnit;
    yUnit = p.yUnit;
    break;
  }

  if (yUnit.length() > 0) {
    if (yUnit.at(yUnit.length() - 1) == '\n')
      yUnit.chop(1);
  }

  if (file.length() > 0)
    fileName = QFileInfo(file).fileName();

  return Data{fileName.toStdString(),
              "",
              file.toStdString(),
              xType.toStdString(),
              yType.toStdString(),
              xUnit.toStdString(),
              yUnit.toStdString(),
              std::move(csvData.data)};
}

CsvFileLoader::Parameters makeCsvLoaderParameters(LoadCsvFileDialog *dlg)
{
  BackendHelpers::showWindowOnTop(dlg);

  while (true) {
    int dlgRet = dlg->exec();
    if (dlgRet != QDialog::Accepted)
      return CsvFileLoader::Parameters();

    const LoadCsvFileDialog::Parameters p = dlg->parameters();

    if (p.delimiter.length() != 1 && (p.delimiter.compare("\\t") != 0)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter must be a single character or '\\t' to represent TAB."));
      continue;
    }
    if (p.decimalSeparator == p.delimiter) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter and decimal separator cannot be the same character."));
      continue;
    }

    if (p.xColumn == p.yColumn) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("X and Y columns cannot be the same"));
      continue;
    }

    QChar delimiter;
    if (p.delimiter.compare("\\t") == 0)
      delimiter = '\t';
    else
      delimiter = p.delimiter.at(0);

    return CsvFileLoader::Parameters(delimiter, p.decimalSeparator,
                                     p.xColumn, p.yColumn,
                                     p.header != LoadCsvFileDialog::HeaderHandling::NO_HEADER,
                                     p.linesToSkip,
                                     p.encodingId, p.readBom);
  }
}

LoaderBackend::~LoaderBackend()
{
}

CSVSupport::CSVSupport()
{
  m_loadCsvFileDlg = new LoadCsvFileDialog{};

  for (const CsvFileLoader::Encoding &enc : CsvFileLoader::SUPPORTED_ENCODINGS)
     m_loadCsvFileDlg->addEncoding(enc.name, enc.displayedName, enc.canHaveBom);
}

CSVSupport::~CSVSupport()
{
  delete m_loadCsvFileDlg;
}

void CSVSupport::destroy()
{
  delete s_me;
}

Identifier CSVSupport::identifier() const
{
  return s_identifier;
}

CSVSupport *CSVSupport::instance()
{
  if (s_me == nullptr) {
    try {
      s_me = new CSVSupport{};
    } catch (...) {
      s_me = nullptr;
    }
  }

  return s_me;
}

std::vector<Data> CSVSupport::load(const int option)
{
  switch (option) {
  case 0:
    return loadCsvFromFile("");
  case 1:
    return loadCsvFromClipboard();
  default:
    return std::vector<Data>();
  }
}

std::vector<Data> CSVSupport::loadCsvFromClipboard()
{
  while (true) {
    CsvFileLoader::Parameters readerParams = makeCsvLoaderParameters(m_loadCsvFileDlg);
    if (!readerParams.isValid)
      return std::vector<Data>{};

    CsvFileLoader::Data csvData = CsvFileLoader::readClipboard(readerParams);
    if (!csvData.isValid()) {
      readerParams = CsvFileLoader::Parameters();
      continue;
    }

    return std::vector<Data>{loadCsvData(csvData, QString(), m_loadCsvFileDlg->parameters())};
  }
}

std::vector<Data> CSVSupport::loadCsvFromFile(const std::string &sourcePath)
{
  QStringList files;
  QFileDialog openDlg(nullptr, QObject::tr("Pick a text data file"), QString::fromStdString(sourcePath));

  BackendHelpers::showWindowOnTop(&openDlg);
  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFiles);

  if (openDlg.exec() != QDialog::Accepted)
     return std::vector<Data>{};

  files = openDlg.selectedFiles();
  if (files.length() < 1)
    return std::vector<Data>{};

  return loadCsvFromFileInternal(files);

}

std::vector<Data> CSVSupport::loadCsvFromFileInternal(const QStringList &files)
{
  std::vector<Data> retData;
  CsvFileLoader::Parameters readerParams;

  for (const QString &f : files) {
    while (true) {
      if (!readerParams.isValid) {
         readerParams = makeCsvLoaderParameters(m_loadCsvFileDlg);
         if (!readerParams.isValid)
           break;
       }

       CsvFileLoader::Data csvData = CsvFileLoader::readFile(f, readerParams);
       if (!csvData.isValid()) {
         readerParams = CsvFileLoader::Parameters();
         continue;
       }

       retData.emplace_back(loadCsvData(csvData, f, m_loadCsvFileDlg->parameters()));
       break;
    }
  }

  return retData;
}

std::vector<Data> CSVSupport::loadHint(const std::string &hintPath, const int option)
{
  switch (option) {
  case 0:
    return loadCsvFromFile(hintPath);
  case 1:
    return loadCsvFromClipboard();
  default:
    return std::vector<Data>{};
  }
}

std::vector<Data>  CSVSupport::loadPath(const std::string &path, const int option)
{
  switch (option) {
  case 0:
    return loadCsvFromFileInternal(QStringList{QString::fromStdString(path)});
  case 1:
    return loadCsvFromClipboard();
  default:
    return std::vector<Data>{};
  }
}

LoaderBackend * initialize()
{
  return CSVSupport::instance();
}

} // namespace backend
