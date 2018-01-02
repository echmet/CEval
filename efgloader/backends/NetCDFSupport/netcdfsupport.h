#ifndef NETCDFSUPPORT_H
#define NETCDFSUPPORT_H

#include "netcdfsupport_global.h"
#include "../../efgloader-core/common/backendinterface.h"

namespace backend {

class NETCDFSUPPORTSHARED_EXPORT NetCDFSupport : public LoaderBackend
{
public:
  virtual Identifier identifier() const override;
  virtual void destroy() override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static NetCDFSupport *initialize(UIBackend *backend);
  static NetCDFSupport *instance();

private:
  NetCDFSupport(UIBackend *backend);
  virtual ~NetCDFSupport() override;
  std::vector<Data> loadInternal(const QString &path);
  Data loadOneFile(const QString &path);

  UIBackend *m_uiBackend;

  static const Identifier s_identifier;
  static NetCDFSupport *s_me;
};

extern "C" {
  NETCDFSUPPORTSHARED_EXPORT LoaderBackend * initialize(UIBackend *backend);
}

} // namespace backend

#endif // NETCDFSUPPORT_H
