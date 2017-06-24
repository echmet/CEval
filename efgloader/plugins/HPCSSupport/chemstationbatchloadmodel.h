#ifndef CHEMSTATIONBATCHLOADMODEL_H
#define CHEMSTATIONBATCHLOADMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include "chemstationfileloader.h"
#include "chemstationbatchloader.h"

class ChemStationBatchLoadModel : public QAbstractItemModel
{
public:
  class Entry {
  public:
    Entry(const QString &type, const QString &info, const ChemStationBatchLoader::Filter &filter);
    Entry(const Entry &other);
    Entry();

    const QString type;
    const QString info;
    const ChemStationBatchLoader::Filter filter;

    Entry &operator=(const Entry &other);
  };

  ChemStationBatchLoadModel(QObject *parent = nullptr);

  void clear();
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  ChemStationBatchLoader::Filter filter(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  int rowCount(const QModelIndex &parent) const override;
  void setNewData(const QVector<Entry> &entries);
  void sort(int column, Qt::SortOrder order) override;

private:
  QVector<Entry> m_entries;

};

#endif // CHEMSTATIONBATCHLOADMODEL_H
