#ifndef CUSTOMLOCALESORTPROXYMODEL_H
#define CUSTOMLOCALESORTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QCollator>

class CustomLocaleSortProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit CustomLocaleSortProxyModel(QObject *parent = nullptr, const QLocale locale = QLocale::system());
  QLocale currentLocale() const;
  void setCurrentLocale(const QLocale locale);
  void setSortCaseSensitivity(Qt::CaseSensitivity cs);

protected:
  virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
  QCollator m_collator;

};

#endif // CUSTOMLOCALESORTPROXYMODEL_H
