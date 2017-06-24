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
  explicit Identifier(const std::string &description, const std::string &tag) :
    description(description),
    tag(tag)
  {}
  const std::string description;
  const std::string tag;
};

class LoaderBackend {
public:
  virtual ~LoaderBackend();
  virtual Identifier identifier() const = 0;
  virtual std::vector<Data> load() = 0;
  virtual std::vector<Data> loadHint(const std::string &hintPath) = 0;
  virtual std::vector<Data> loadPath(const std::string &path) = 0;
};

typedef LoaderBackend *(*BackendInitializer)();

} // namespace backend

#endif // BACKENDINTERFACE_H
