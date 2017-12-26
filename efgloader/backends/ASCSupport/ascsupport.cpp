#include "ascsupport.h"
#include "../../efgloader-core/common/backendhelpers_p.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale>
#include <iostream>
#include <regex>
#include <QComboBox>
#include <QLayout>

#if defined Q_OS_UNIX
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <cstring>
#elif defined Q_OS_WIN
  #include <Windows.h>
#endif // Q_OS_

#define KV_DELIM ':'

static const std::string HZ_STR{"Hz"};
static const std::string PTS_STR{"Pts."};

namespace backend {

typedef std::list<std::string>::const_iterator LIt;

char initDecimalPoint()
{
  static const std::locale sLoc{""};
  const char sep = std::use_facet<std::numpunct<char>>(sLoc).decimal_point();

#ifdef Q_OS_WIN
  setlocale(LC_ALL, sLoc.c_str()); /* No, this is not a joke */
#endif // Q_OS_WIN

  return sep;
}

double strToDbl(const std::string &ns)
{
  static const char decPoint = initDecimalPoint();

  std::string s{ns};

  auto replaceAll = [&s](const char current, const char wanted) {
    const std::string _wanted{wanted};

    size_t idx = s.find(current);
    while (idx != s.npos) {
      s = s.replace(idx , 1, _wanted);
      idx = s.find(current);
    }
  };

  switch (decPoint) {
  case '.':
    replaceAll(',', '.');
    break;
  case ',':
    replaceAll('.', ',');
    break;
  }

  double d;
  try {
    size_t invIdx;
    d = std::stod(s, &invIdx);
    if (invIdx != s.length())
      throw ASCFormatException{"String \"" + ns + "\" cannot be converted to decimal number"};
  } catch (std::invalid_argument &) {
    throw ASCFormatException{"String \"" + ns + "\" cannot be converted to decimal number"};
  } catch (std::out_of_range &) {
    throw ASCFormatException{"String \"" + ns + "\" represents a number outside of the available numerical range"};
  }

  return d;
}

int strToInt(const std::string &s)
{
  try {
    size_t invIdx;
    int n = std::stoi(s, &invIdx);
    if (invIdx != s.length())
      throw ASCFormatException{"String \"" + s + "\" cannot be converted to integer"};
    return n;
  } catch (std::invalid_argument &) {
    throw ASCFormatException{"String \"" + s + "\" cannot be converted to integer"};
  } catch (std::out_of_range &) {
    throw ASCFormatException{"String \"" + s + "\" represents a number outside of the available numerical range"};
  }
}

void reportError(const QString &error)
{
  QMessageBox mbox{QMessageBox::Critical, QObject::tr("Cannot read ASC file"), error};
  mbox.exec();
}

void reportWarning(const QString &warning)
{
  QMessageBox mbox{QMessageBox::Warning, QObject::tr("Cannot read ASC file"), warning};
  mbox.exec();
}

std::vector<std::string> fileList(const QString &hintPath)
{
  std::vector<std::string> files{};
  QFileDialog openDlg{nullptr, QObject::tr("Pick an ASC file"), hintPath};

  openDlg.setNameFilter("ASC file (*.asc *.ASC)");
  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFiles);

  BackendHelpers::showWindowOnTop(&openDlg);
  if  (openDlg.exec() != QDialog::Accepted)
    return files;

  const auto _files = openDlg.selectedFiles();

  for (const QString &file : _files)
    files.emplace_back(QDir::toNativeSeparators(file).toUtf8());

  return files;
}

std::tuple<std::string, std::string> splitKeyValue(const std::string &entry, const std::string &delim)
{
  const size_t delimIdx = entry.find(delim);
  const size_t STEP = delim.length();

  if (delimIdx == entry.npos)
    throw std::runtime_error{"KeyValue entry contains no delimiter"};

  std::string key = entry.substr(0, delimIdx);
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);

  if (entry.length() <= delimIdx + STEP)
    return {key, ""};
  else
    return {key, entry.substr(delimIdx + delim.length())};
}

char pickDecimalPoint()
{
  QDialog dlg{};

  QComboBox qcbox{};
  qcbox.addItem("Comma (,)", ',');
  qcbox.addItem("Period (.)", '.');

  dlg.setLayout(new QVBoxLayout{});

  dlg.layout()->addWidget(&qcbox);

  dlg.exec();

  return qcbox.currentData().toChar().toLatin1();
}

ASCContext makeContext(const std::string &name, const std::string &path, const std::list<std::string> &header)
{
  int nChans = -1;
  std::string kvDelim;

  /* Autodetect the value delimiter first */
  const char valueDelim = [&header] {
    const std::string &firstLine = header.front();
    const size_t delimIdx = firstLine.find(KV_DELIM);

    if (delimIdx == firstLine.npos)
      throw ASCFormatException{"KeyValue entry contains no delimiter"};

    if (firstLine.length() == delimIdx + 1)
      throw ASCFormatException{"First line in the header contains a key with no value"};

    return firstLine.at(delimIdx + 1);
  }();

  kvDelim = std::string{KV_DELIM} + std::string{valueDelim};
  const char dataDecimalPoint = [](const char valueDelim) {
    if (valueDelim == '.' || valueDelim == ',')
      return pickDecimalPoint();

    return '.'; /* The actual value does not really matter */
  }(valueDelim);

  /* Look for the number of channels in the file */
  for (LIt it = header.cbegin(); it != header.cend(); it++) {
    const auto kv = splitKeyValue(*it, kvDelim);
    const std::string &key = std::get<0>(kv);
    const std::string &value = std::get<1>(kv);

    if (key == "maxchannels") {
      try {
        nChans = strToInt(value);
        break;
      } catch (ASCFormatException &) {
        throw ASCFormatException{"Cannot read number of channels"};
      }
    }
  }

  if (nChans < 1)
    throw ASCFormatException{"Number of channels in the file is either not specified or invalid"};

  return ASCContext{name, path, static_cast<size_t>(nChans), std::move(kvDelim), valueDelim, dataDecimalPoint};
}

void parseTraces(std::vector<Data> &data, const ASCContext &ctx, const std::list<std::string> &traces)
{
  LIt it = traces.cbegin();

  for (size_t channel = 0; channel < ctx.nChans; channel++) {
    const int numPoints = ctx.nDatapoints.at(channel);
    const double timeStep = 1.0 / ctx.samplingRates.at(channel) * ctx.xAxisMultipliers.at(channel);
    const double yMultiplier = ctx.yAxisMultipliers.at(channel);
    std::vector<std::tuple<double, double>> dataPoints{};

    dataPoints.reserve(numPoints);

    for (int pt = 0; pt < numPoints; pt++) {
      const std::string &s = *it;
      const double xValue = timeStep * pt;
      const double yValue = strToDbl(s) * yMultiplier;
      dataPoints.emplace_back(xValue, yValue);

      if ((++it == traces.cend()) && (pt != numPoints - 1))
        throw ASCFormatException{"Unexpected end of data trace"};
    }

    data.emplace_back(Data{ctx.name, std::string{"Channel "} + std::to_string(channel),
                           ctx.path,
                           "Time",
                           "Signal",
                           ctx.xAxisTitles.at(channel),
                           ctx.yAxisTitles.at(channel),
                           std::move(dataPoints)
                      });
  }

  if (it != traces.cend())
    reportWarning("Data trace is longer than expected");
}

#if defined Q_OS_UNIX
std::istringstream readFile(const std::string &path)
{
  int fd = open(path.c_str(), O_RDONLY);
  if (fd <= 0) {
    const std::string errStr{strerror(errno)};
    throw ASCFormatException{std::string{"Cannot open file: "} + errStr};
  }

  const off_t len = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  char *buf = new char[len + 1];
  if (buf == nullptr) {
    close(fd);
    throw ASCFormatException{"Cannot open file: out of memory"};
  }

  ssize_t r = read(fd, buf, len);
  if (r < len) {
    delete [] buf;
    close(fd);
    throw ASCFormatException{"Cannot open file: unable to read the whole file"};
  }
  buf[len] = '\0';

  close(fd);
  std::istringstream iss{std::string{buf}};

  delete [] buf;

  return iss;
}
#elif defined Q_OS_WIN
std::istringstream readFile(const std::string &path)
{
  HANDLE fh;
  int wSize;
  wchar_t *wPath;
  DWORD fileSizeHigh;
  char *buf;
  int64_t fileSize = 0;

  wSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, nullptr, 0);
  if (wSize <= 0)
    throw ASCFormatException{"Cannot get wchar_t array size"};

  wPath = new wchar_t[wSize];
  if (wPath == nullptr)
    throw ASCFormatException{"Cannot convert UTF-8 path to Windows native path: Out of memory"};

  wSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path.c_str(), -1, wPath, wSize);
  if (wSize <= 0) {
    delete [] wPath;
    throw ASCFormatException{"Cannot convert UTF-8 path to Windows native path: Conversion failure"};
  }

  fh = CreateFileW(wPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fh == INVALID_HANDLE_VALUE) {
    delete [] wPath;
    throw ASCFormatException{"Cannot open data file"};
  }

  const DWORD fileSizeLow = GetFileSize(fh, &fileSizeHigh);
  if (fileSizeLow == INVALID_FILE_SIZE) {
    CloseHandle(fh);
    delete [] wPath;
    throw ASCFormatException{"Failed to get file size"};
  }

  fileSize |= static_cast<int64_t>(fileSizeHigh) << 32 | fileSizeLow;

  buf = new char[fileSize + 1];
  if (buf == nullptr) {
    CloseHandle(fh);
    delete [] wPath;
    throw ASCFormatException{"Cannot allocate input buffer"};
  }

  DWORD bytesRead;
  if (ReadFile(fh, buf, fileSize, &bytesRead, NULL) != TRUE) {
    CloseHandle(fh);
    delete [] buf;
    delete [] wPath;
    throw ASCFormatException{"Cannot read file content"};
  }
  buf[fileSize] = '\0';


  /* We fail if the file size exceeds 4 GiB */
  if (bytesRead != fileSize) {
    CloseHandle(fh);
    delete [] buf;
    delete [] wPath;
    throw ASCFormatException{"Number of read bytes does not match the size of the file"};
  }

  std::istringstream iss{buf};
  CloseHandle(fh);
  delete [] buf;
  delete [] wPath;

  return iss;
}

#else
#error "Unknown platform"
#endif // Q_OS_

std::string readLine(std::istringstream &stream)
{
  auto endOfLine = [&stream]() {
    int c = stream.peek();
    char _c;

    if (c < 0) return true;
    if (c == '\r') {
      stream.get(_c);
      c = stream.peek();
      if (c == '\n') {
        stream.get(_c);
        return true;    /* CRLF - Windows */
      }
      return true;      /* CR - Old MacOS */
    } else if (c == '\n') {
      stream.get(_c);
      return true;      /* LF - Unix */
    }
    return false;
  };

  std::string line{};

  while (!endOfLine()) {
    char c;
    stream.get(c);
    line += c;
  }

  return line;
}

void spliceHeaderTraces(std::list<std::string> &lines, std::list<std::string> &header, std::list<std::string> &traces)
{
  LIt it = lines.cbegin();
  for (; it != lines.cend(); it++) {
    const auto &l = *it;

    if (l.find(KV_DELIM) == l.npos)
      break;
  }

  header.splice(header.cbegin(), lines, lines.cbegin(), it);

  traces = std::move(lines);
}

std::vector<std::string> split(std::string s, const char delim)
{
  std::vector<std::string> pieces;
  size_t delimIdx;

  do {
    delimIdx = s.find(delim);
    pieces.emplace_back(s.substr(0, delimIdx));
    s = s.substr(delimIdx + 1);
  } while (delimIdx != s.npos);

  return pieces;
}

std::vector<std::string> splitDecimal(std::string s, const char delim, const char dataDecimalPoint)
{
  static const std::regex sciNotation{"^([-+]?[0-9]+)[eE]{1}[-+]?[0-9]+$"};

  auto extractSegment = [&s](std::string &seg, const char delim) {
    size_t idx = -1;
    int cnt = 0;
    while (cnt < 2) {
      idx = s.find(delim, idx + 1);
      if (idx == s.npos)
        break;
      cnt++;
    }

    seg = s.substr(0, idx);
    s = s.substr(idx + 1);

    return cnt == 2;
  };

  if (delim != dataDecimalPoint)
    return split(s, delim);

  std::vector<std::string> segments{};
  while (true) {
    std::string seg{};
    const bool cont = extractSegment(seg, delim);

    const auto sep = split(seg, delim);
    if (sep.size() > 2)
      throw std::logic_error{"Incorrect string parsing in \"splitDecimal\" detected"};
    if (std::regex_match(sep.at(0), sciNotation)) {
      segments.emplace_back(sep.at(0));
      if (!cont) {
        if (sep.size() == 2)
          segments.emplace_back(sep.at(1));
      } else
        s = sep.at(1) + delim + s;
    } else
      segments.emplace_back(std::move(seg));
    if (!cont)
      break;
  }

  return segments;
}

Identifier ASCSupport::s_identifier{"ASC text file (EZChrom format)", "ASC text", "ASC", {}};
ASCSupport *ASCSupport::s_me{nullptr};

/* Format-specific handler implementations */
void EZChrom_SamplingRate(std::vector<double> &rates, const char delim, const char decPoint, const std::string &entry)
{
  const std::vector<std::string> parts = splitDecimal(entry, delim, decPoint);

  if (parts.back() != HZ_STR)
    throw ASCFormatException{"Unexpected unit of sampling rate"};

  for (size_t idx = 0; idx < parts.size() - 1; idx++)
    rates.emplace_back(strToDbl(parts.at(idx)));
}

void EZChrom_TotalDataPoints(std::vector<int> &rates, const char delim, const std::string &entry)
{
  std::vector<std::string> parts = split(entry, delim);

  if (delim == '.') {
    parts.pop_back();
    parts.back() += ".";
  }

  if (parts.back() != PTS_STR)
    throw ASCFormatException{"Unexpected unit of total data points"};

  for (size_t idx = 0; idx < parts.size() - 1; idx++)
    rates.emplace_back(strToInt(parts.at(idx)));
}

void EZChrom_AxisMultiplier(std::vector<double> &multipliers, const char delim, const char decPoint, const std::string &entry)
{
  const std::vector<std::string> parts = splitDecimal(entry, delim, decPoint);

  for (const std::string &s : parts)
    multipliers.emplace_back(strToDbl(s));
}

void EZChrom_AxisTitle(std::vector<std::string> &titles, const char delim, const std::string &entry)
{
  titles = split(entry, delim);
}

/* Handlers */
EntryHandlersMap ASCSupport::s_handlers =
{
  {
    EntryHandlerSamplingRate::ID,
    new EntryHandlerSamplingRate(EZChrom_SamplingRate)
  },
  {
    EntryHandlerTotalDataPoints::ID,
    new EntryHandlerTotalDataPoints(EZChrom_TotalDataPoints)
  },
  {
    EntryHandlerXAxisMultiplier::ID,
    new EntryHandlerXAxisMultiplier(EZChrom_AxisMultiplier)
  },
  {
    EntryHandlerYAxisMultiplier::ID,
    new EntryHandlerYAxisMultiplier(EZChrom_AxisMultiplier)
  },
  {
    EntryHandlerXAxisTitle::ID,
    new EntryHandlerXAxisTitle(EZChrom_AxisTitle)
  },
  {
    EntryHandlerYAxisTitle::ID,
    new EntryHandlerYAxisTitle(EZChrom_AxisTitle)
  }

};

LoaderBackend::~LoaderBackend()
{
}

ASCSupport::ASCSupport()
{
}

ASCSupport::~ASCSupport()
{
}

void ASCSupport::destroy()
{
  delete s_me;
}

const EntryHandler * ASCSupport::getHandler(const std::string &key)
{
  class EntryHandlerNull : public EntryHandlerEssentalityTrait<false> { public: virtual void process(ASCContext &, const std::string&) const override {} };
  typedef EntryHandlersMap::const_iterator EHMIt;

  static EntryHandlerNull *nullHandler = new EntryHandlerNull{};

  EHMIt it = s_handlers.find(key);
  if (it == s_handlers.cend())
    return nullHandler;
  else
    return it->second;
}

Identifier ASCSupport::identifier() const
{
  return s_identifier;
}

ASCSupport * ASCSupport::instance()
{
  if (s_me == nullptr)
    s_me = new (std::nothrow) ASCSupport{};

  return s_me;
}

std::vector<Data> ASCSupport::load(const int option)
{
  (void)option;

  return loadInteractive("");
}

std::vector<Data> ASCSupport::loadHint(const std::string &hintPath, const int option)
{
  (void)option;

  return loadInteractive(hintPath);
}

std::vector<Data> ASCSupport::loadPath(const std::string &path, const int option)
{
  (void)option;

  return loadInternal(path);
}

std::vector<Data> ASCSupport::loadInteractive(const std::string &hintPath)
{
  std::vector<Data> data{};
  std::vector<std::string> files = fileList(QString::fromStdString(hintPath));

  if (files.size() == 0)
    return data;

  for (const std::string &file : files) {
    const auto _data = loadInternal(file);
    std::copy(_data.cbegin(), _data.cend(), std::back_inserter(data));
  }

  return data;
}

std::vector<Data> ASCSupport::loadInternal(const std::string &path)
{
  std::vector<Data> data{};

  std::list<std::string> lines{};
  std::istringstream inStream{};

  try {
    inStream = readFile(path);
  } catch (const ASCFormatException &ex) {
    reportError(ex.what());
    return data;
  }

  while (inStream.good()) {
    std::string line = readLine(inStream);
    if (line.length() > 0)
      lines.emplace_back(std::move(line));
  }

  if (!inStream.eof()) {
    reportError("I/O error while reading ASC file");
    return data;
  }

  std::list<std::string> header{};
  std::list<std::string> traces{};
  spliceHeaderTraces(lines, header, traces);

  if (header.size() < 1) {
    reportError("File contains no header"); /* TODO: Switch to headerless mode */
    return data;
  }

  try {
    std::string name = [&path]() -> std::string {
        return {std::find_if(path.rbegin(), path.rend(),
                             [](char c) { return c == '/' || c == '\\'; }).base(),
                path.end()};
    }();

    ASCContext ctx = makeContext(name, path, header);

    parseHeader(ctx, header);
    parseTraces(data, ctx, traces);
  } catch (ASCFormatException &ex) {
    reportError(QString::fromStdString(ex.what()));
    return std::vector<Data>{};
  } catch (std::bad_alloc &) {
    reportError(QObject::tr("Insufficient memory to read ASC file"));
    return std::vector<Data>{};
  }

  return data;
}

void ASCSupport::parseHeader(ASCContext &ctx, const std::list<std::string> &header)
{
  for (LIt it = header.cbegin(); it != header.cend(); it++) {
    const auto kv = splitKeyValue(*it, ctx.kvDelim);

    const std::string &key = std::get<0>(kv);
    const std::string &value = std::get<1>(kv);

    const EntryHandler *handler = getHandler(key);
    try {
      handler->process(ctx, value);
    } catch (ASCFormatException &ex) {
      if (handler->essential()) {
        throw;
      } else {
        reportWarning(QString::fromStdString(ex.what()));
      }
    } catch (std::bad_alloc &) {
      throw;
    }
  }

  if (!ctx.validate())
    throw ASCFormatException{"ASC header does not contain all information necessary to interpret the data"};
}

LoaderBackend * initialize()
{
  return ASCSupport::instance();
}

} //namespace backend
