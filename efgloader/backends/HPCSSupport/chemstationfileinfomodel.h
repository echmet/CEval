#ifndef CHEMSTATIONFILEINFOMODEL_H
#define CHEMSTATIONFILEINFOMODEL_H

#include <QAbstractItemModel>

class ChemStationFileInfoModel : public QAbstractItemModel
{
public:
  class Entry {
  public:
    Entry(const QString &name, const QString &type, const QString &info, const bool isChFile);
    Entry(const Entry &other);
    Entry();

    const QString name;
    const QString type;
    const QString info;
    const bool isChFile;

    Entry &operator=(const Entry &other);
  };

  ChemStationFileInfoModel(QObject *parent = nullptr);
  void clear();
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  int rowCount(const QModelIndex &parent) const override;
  void setNewData(const QVector<Entry> &entries);
  void sort(int column, Qt::SortOrder order) override;

private:
  QVector<Entry> m_entries;

};
#endif // CHEMSTATIONFILEINFOMODEL_H
