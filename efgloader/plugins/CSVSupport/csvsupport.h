#ifndef CSVSUPPORT_H
#define CSVSUPPORT_H

#include "../../core/common/backendinterface.h"
#include "csvsupport_global.h"

namespace backend {

class CSVSUPPORTSHARED_EXPORT CSVSupport : public LoaderBackend {
public:
  virtual Identifier identifier() const override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static CSVSupport *instance();

private:
  CSVSupport();

  static CSVSupport *s_me;
  static Identifier s_identifier;
};

extern "C" {
  CSVSUPPORTSHARED_EXPORT LoaderBackend * initialize();
}

} // namespace backend

#endif // CSVSUPPORT_H
