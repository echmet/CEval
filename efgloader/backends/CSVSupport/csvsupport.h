#ifndef CSVSUPPORT_H
#define CSVSUPPORT_H

#include "../../efgloader-core/common/backendinterface.h"
#include "csvsupport_global.h"

class QStringList;

namespace backend {

class LoadCsvFileDialog;

class CSVSUPPORTSHARED_EXPORT CSVSupport : public LoaderBackend {
public:
  virtual Identifier identifier() const override;
  virtual void destroy() override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static CSVSupport *instance(UIBackend *backend);

private:
  CSVSupport(UIBackend *backend);
  virtual ~CSVSupport() override;
  std::vector<Data> loadCsvFromClipboard();
  std::vector<Data> loadCsvFromFile(const std::string &sourcePath);
  std::vector<Data> loadCsvFromFileInternal(const QStringList &files);

  UIBackend *m_uiBackend;

  static CSVSupport *s_me;
  static const Identifier s_identifier;
};

extern "C" {
  CSVSUPPORTSHARED_EXPORT LoaderBackend * initialize(UIBackend *backend);
}

} // namespace backend

#endif // CSVSUPPORT_H
