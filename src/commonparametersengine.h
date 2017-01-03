#ifndef COMMONPARAMETERSENGINE_H
#define COMMONPARAMETERSENGINE_H

#include <QObject>
#include "commonparametersitems.h"
#include "booleanmappermodel.h"
#include "floatingmappermodel.h"
#include "mappedvectorwrapper.h"

class CommonParametersEngine : public QObject
{
  Q_OBJECT
public:
  class Context {
  public:
    Context(const MappedVectorWrapper<double, CommonParametersItems::Floating> &numData,
            const MappedVectorWrapper<bool, CommonParametersItems::Boolean> &boolData);
    Context();
    bool isValid() const;

    const MappedVectorWrapper<double, CommonParametersItems::Floating> numData;
    const MappedVectorWrapper<bool, CommonParametersItems::Boolean> boolData;

  private:
    bool m_valid;
  };

  explicit CommonParametersEngine(QObject *parent = nullptr);
  AbstractMapperModel<bool, CommonParametersItems::Boolean> *boolModel();
  bool boolValue(const CommonParametersItems::Boolean item) const;
  Context currentContext() const;
  AbstractMapperModel<double, CommonParametersItems::Floating> *numModel();
  double numValue(const CommonParametersItems::Floating item) const;
  void revalidate() const;
  bool setContext(const Context &ctx);

private:
  FloatingMapperModel<CommonParametersItems::Floating> m_numModel;
  BooleanMapperModel<CommonParametersItems::Boolean> m_boolModel;
  MappedVectorWrapper<double, CommonParametersItems::Floating> m_numData;
  MappedVectorWrapper<bool, CommonParametersItems::Boolean> m_boolData;

  void checkValidity() const;
  bool recalculate();

signals:
  void noEofStateChanged(const bool noEof);
  void parametersUpdated();
  void validityState(const bool state, const CommonParametersItems::Floating item) const;

public slots:
    void onNoEofStateChanged();
    void onUpdateTEof(const double t);

private slots:
  void onNumModelDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);

};

#endif // COMMONPARAMETERSENGINE_H
