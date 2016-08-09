#ifndef CHEMSTATIONBATCHLOADMODEL_H
#define CHEMSTATIONBATCHLOADMODEL_H

#include <QAbstractItemModel>

class ChemStationBatchLoadModel : public QAbstractItemModel
{
public:
  ChemStationBatchLoadModel(QObject *parent = nullptr);
};

#endif // CHEMSTATIONBATCHLOADMODEL_H
