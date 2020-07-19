#ifndef EVALUATEDPEAKSMODEL_H
#define EVALUATEDPEAKSMODEL_H

#include <memory>
#include <QAbstractItemModel>

class EvaluatedPeaksModel : public QAbstractItemModel
{
public:
  class EvaluatedPeak {
  public:
    explicit EvaluatedPeak();
    explicit EvaluatedPeak(const QString &name, const double time, const double area);
    EvaluatedPeak(const EvaluatedPeak &other);
    const QString name;
    const double time;
    const double area;

    EvaluatedPeak &operator=(const EvaluatedPeak &other);
  };

  explicit EvaluatedPeaksModel(QObject *parent = nullptr);
  bool appendEntry(const EvaluatedPeak &peak);
  void clearEntries();
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  void removeEntry(const int idx);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  void setEntries(const QVector<EvaluatedPeak> &entries);
  void updateEntry(const int idx, const EvaluatedPeak &peak);
  void updateEntry(const int idx, const double time, const double area);
  void updateName(const int idx, const QString &name);

private:
  QVector<EvaluatedPeak> m_data;

};

#endif // EVALUATEDPEAKSMODEL_H
