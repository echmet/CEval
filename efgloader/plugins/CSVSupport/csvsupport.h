#ifndef CSVSUPPORT_H
#define CSVSUPPORT_H

#include "../../core/common/backendinterface.h"
#include "csvsupport_global.h"

class QStringList;

namespace backend {

class LoadCsvFileDialog;

class CSVSUPPORTSHARED_EXPORT CSVSupport : public LoaderBackend {
public:
  virtual Identifier identifier() const override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static CSVSupport *instance();
  static void destroy();

private:
  CSVSupport();
  ~CSVSupport();
  std::vector<Data> loadCsvFromClipboard();
  std::vector<Data> loadCsvFromFile(const std::string &sourcePath);
  std::vector<Data> loadCsvFromFileInternal(const QStringList &files);

  LoadCsvFileDialog *m_loadCsvFileDlg;

  static CSVSupport *s_me;
  static Identifier s_identifier;
};

extern "C" {
  CSVSUPPORTSHARED_EXPORT LoaderBackend * initialize();
  CSVSUPPORTSHARED_EXPORT void destroy();
}

} // namespace backend

#endif // CSVSUPPORT_H
