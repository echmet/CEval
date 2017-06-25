#ifndef BACKENDINTERFACE_H
#define BACKENDINTERFACE_H

#include <string>
#include <tuple>
#include <vector>

namespace backend {

class Data {
public:
  std::string name;
  std::string path;
  std::string xDescription;
  std::string yDescription;
  std::string xUnit;
  std::string yUnit;
  std::vector<std::tuple<double, double>> datapoints;
};

class Identifier {
public:
  explicit Identifier(const std::string &longDescription, const std::string &shortDescription, const std::string &tag, const std::vector<std::string> &loadOptions) :
    longDescription(longDescription),
    shortDescription(shortDescription),
    tag(tag),
    loadOptions(loadOptions)
  {}
  const std::string longDescription;
  const std::string shortDescription;
  const std::string tag;
  const std::vector<std::string> loadOptions;
};

class LoaderBackend {
public:
  virtual ~LoaderBackend() = 0;
  virtual Identifier identifier() const = 0;
  virtual std::vector<Data> load(const int option) = 0;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) = 0;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) = 0;
};

typedef LoaderBackend *(*BackendInitializer)();

} // namespace backend

#endif // BACKENDINTERFACE_H
