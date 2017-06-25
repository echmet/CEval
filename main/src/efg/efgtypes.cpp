#include "efgtypes.h"

EFGData::EFGData(const QVector<QPointF> data, const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit) :
  data(data),
  xType(xType),
  xUnit(xUnit),
  yType(yType),
  yUnit(yUnit),
  m_valid(true)
{
}

EFGData::EFGData() :
  m_valid(false)
{
}

EFGData::EFGData(const EFGData &other) :
  data(other.data),
  xType(other.xType),
  xUnit(other.xUnit),
  yType(other.yType),
  yUnit(other.yUnit),
  m_valid(other.m_valid)
{
}

EFGData &EFGData::operator=(const EFGData &other)
{
  const_cast<QVector<QPointF>&>(data) = other.data;
  const_cast<QString&>(xType) = other.xType;
  const_cast<QString&>(xUnit) = other.xUnit;
  const_cast<QString&>(yType) = other.yType;
  const_cast<QString&>(yUnit) = other.xUnit;
  const_cast<bool&>(m_valid) = other.m_valid;

  return *this;
}

EFGSupportedFileFormat::EFGSupportedFileFormat() :
  longDescription(""),
  shortDescription(""),
  formatTag("")
{
}

EFGSupportedFileFormat::EFGSupportedFileFormat(const QString &longDescription, const QString &shortDescription, const QString &formatTag) :
  longDescription(longDescription),
  shortDescription(shortDescription),
  formatTag(formatTag)
{
}
