#ifndef FOUNDPEAKSMODEL_H
#define FOUNDPEAKSMODEL_H

#include <QAbstractItemModel>

class FoundPeaksModel : public QAbstractItemModel
{
public:
  class Peak {
  public:
    explicit Peak(const double time, const long index);
    explicit Peak(const Peak &other);
    explicit Peak();

    const long index;
    const double time;

    Peak &operator=(const Peak &other);
  };

  explicit FoundPeaksModel(const QVector<Peak> &data, QObject *parent = nullptr);
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child = QModelIndex()) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
  QVector<Peak> m_data;
};

#endif // FOUNDPEAKSMODEL_H
