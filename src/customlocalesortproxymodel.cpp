#include "customlocalesortproxymodel.h"

CustomLocaleSortProxyModel::CustomLocaleSortProxyModel(QObject *parent, const QLocale locale) :
  QSortFilterProxyModel(parent),
  m_collator(QCollator(locale))
{
  m_collator.setNumericMode(true);
}

QLocale CustomLocaleSortProxyModel::currentLocale() const
{
  return m_collator.locale();
}

bool CustomLocaleSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
  const QVariant leftData = sourceModel()->data(source_left, sortRole());
  const QVariant rightData = sourceModel()->data(source_right, sortRole());

  if (leftData.metaType() == QMetaType(QMetaType::QString)) {
    const QString leftString = leftData.toString();
    const QString rightString = rightData.toString();

    if (isSortLocaleAware())
      return m_collator.compare(leftString, rightString) < 0;
    else
      return QString::compare(leftString, rightString, sortCaseSensitivity()) < 0;
  } else
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void CustomLocaleSortProxyModel::setCurrentLocale(const QLocale locale)
{
  m_collator.setLocale(locale);
  invalidate();
}

void CustomLocaleSortProxyModel::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
  m_collator.setCaseSensitivity(cs);
  QSortFilterProxyModel::setSortCaseSensitivity(cs);
}
