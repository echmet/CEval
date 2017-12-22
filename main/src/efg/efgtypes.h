#ifndef EFGTYPES_H
#define EFGTYPES_H

#include <QObject>
#include <QMap>
#include <QMetaType>
#include <QPointF>
#include <QString>
#include <QVector>
#include <memory>

class EFGData {
public:
  explicit EFGData();
  explicit EFGData(const QVector<QPointF> &data, const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit, const QString &dataId);
  explicit EFGData(const EFGData &other);
  bool isValid() const;
  EFGData &operator=(const EFGData &other);

  const QVector<QPointF> data;
  QString xType;
  QString xUnit;
  QString yType;
  QString yUnit;
  QString dataId;
private:
   const bool m_valid;
};

class EFGSupportedFileFormat {
public:
  explicit EFGSupportedFileFormat();
  explicit EFGSupportedFileFormat(const QString &longDescription, const QString &shortDescription, const QString &formatTag, const QMap<int, QString> &loadOptions);

  const QString longDescription;
  const QString shortDescription;
  const QString formatTag;
  const QMap<int, QString> loadOptions;
};

typedef std::shared_ptr<EFGData> EFGDataSharedPtr;
Q_DECLARE_METATYPE(EFGDataSharedPtr)

namespace efg {

class GUIDisplayer : public QObject {
  Q_OBJECT
public slots:
  void onDisplayWarning(const QString &title, const QString &message);
};

} // namespace efg

#endif // EFGTYPES_H
