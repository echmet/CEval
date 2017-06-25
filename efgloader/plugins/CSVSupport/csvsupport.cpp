#include "csvsupport.h"

namespace backend {

CSVSupport *CSVSupport::s_me{nullptr};
Identifier CSVSupport::s_identifier{"Comma separated file format support", "CSV", "CSV", {"file", "clipboard"}};

LoaderBackend::~LoaderBackend()
{
}

CSVSupport::CSVSupport()
{
}

Identifier CSVSupport::identifier() const
{
  return s_identifier;
}

CSVSupport *CSVSupport::instance()
{
  if (s_me == nullptr)
    s_me = new CSVSupport{};

  return s_me;
}

std::vector<Data> CSVSupport::load(const int option)
{
  switch (option) {
  /*case 0:
    return loadCsvFromFile();
  case 1:
    return loadCsvFromClipboard();*/
  default:
    return std::vector<Data>();
  }
}

std::vector<Data> CSVSupport::loadHint(const std::string &hintPath, const int option)
{
  return std::vector<Data>();
}

std::vector<Data>  CSVSupport::loadPath(const std::string &path, const int option)
{
  return std::vector<Data>();
}

LoaderBackend * initialize()
{
  return CSVSupport::instance();
}

} // namespace backend
