#ifndef COMMONPARAMETERSENGINE_H
#define COMMONPARAMETERSENGINE_H

#include <QObject>
#include "commonparametersitems.h"
#include "floatingmappermodel.h"
#include "mappedvectorwrapper.h"

class CommonParametersEngine : public QObject
{
  Q_OBJECT
public:
  class Context {
  public:
    Context(const MappedVectorWrapper<double, CommonParametersItems::Floating> &data);
    Context();
    bool isValid() const;

    const MappedVectorWrapper<double, CommonParametersItems::Floating> data;

  private:
    bool m_valid;
  };

  explicit CommonParametersEngine(QObject *parent = nullptr);
  Context currentContext() const;
  AbstractMapperModel<double, CommonParametersItems::Floating> *model();
  void revalidate() const;
  bool setContext(const Context &ctx);
  double value(const CommonParametersItems::Floating item) const;

private:
  FloatingMapperModel<CommonParametersItems::Floating> m_model;
  MappedVectorWrapper<double, CommonParametersItems::Floating> m_data;

  void checkValidity() const;
  bool recalculate();

signals:
  void parametersUpdated();
  void validityState(const bool state, const CommonParametersItems::Floating item) const;

public slots:
    void onUpdateTEof(const double t);

private slots:
  void onModelDataChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);

};

#endif // COMMONPARAMETERSENGINE_H
