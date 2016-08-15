#ifndef IDATAEXPORTABLE_H
#define IDATAEXPORTABLE_H

#include <functional>
#include <QVariant>

template<typename T>
class IDataExportable {
public:
  typedef std::function<QVariant (const T *)> Retriever;

  QVariant retrieveExportable(Retriever r) const
  {
    return r();
  }

protected:
  IDataExportable() { }

};

#endif // IDATAEXPORTABLE_H
