#ifndef EFGTYPES_H
#define EFGTYPES_H

#include <QPointF>
#include <QString>
#include <QVector>

class EFGData {
public:
  explicit EFGData(const QVector<QPointF> data, const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit);
  explicit EFGData();
  explicit EFGData(const EFGData &other);
  bool isValid() const;
  EFGData &operator=(const EFGData &other);

  const QVector<QPointF> data;
  QString xType;
  QString xUnit;
  QString yType;
  QString yUnit;
private:
   const bool m_valid;
};

class EFGSupportedFileFormat {
public:
  explicit EFGSupportedFileFormat();
  explicit EFGSupportedFileFormat(const QString formatTag, const QString longDescription, const QString shortDescription);

  const QString formatTag;
  const QString longDescription;
  const QString shortDescription;
};

#endif // EFGTYPES_H
