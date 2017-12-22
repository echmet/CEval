#ifndef ASCSUPPORT_H
#define ASCSUPPORT_H

#include "ascsupport_global.h"
#include "ascsupport_handlers.h"
#include "../../efgloader-core/common/backendinterface.h"
#include <list>

namespace backend {

class ASCSUPPORTSHARED_EXPORT ASCSupport : public LoaderBackend {
public:
  virtual Identifier identifier() const override;
  virtual void destroy() override;
  virtual std::vector<Data> load(const int option) override;
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) override;
  virtual std::vector<Data> loadPath(const std::string &path, const int option) override;

  static ASCSupport *instance();

private:
  ASCSupport();
  virtual ~ASCSupport() override;
  const EntryHandler * getHandler(const std::string &key);
  std::vector<Data> loadInteractive(const std::string &hintPath);
  std::vector<Data> loadInternal(const std::string &path);
  void parseHeader(ASCContext &ctx, const std::list<std::string> &header);

  static Identifier s_identifier;
  static ASCSupport *s_me;
  static EntryHandlersMap s_handlers;
};

extern "C" {
  ASCSUPPORTSHARED_EXPORT LoaderBackend * initialize();
}

} // namespace backend

#endif // ASCSUPPORT_H
