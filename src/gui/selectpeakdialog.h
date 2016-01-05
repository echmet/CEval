#ifndef SELECTPEAKDIALOG_H
#define SELECTPEAKDIALOG_H

#include <QAbstractItemModel>
#include <QDialog>

namespace Ui {
class SelectPeakDialog;
}

class SelectPeakDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SelectPeakDialog(QWidget *parent = nullptr);
  ~SelectPeakDialog();
  void bindModel(QAbstractItemModel *model);
  int selectedPeak() const;
  void setPeakWindow(const long peakWindow);

private:
  void closeEvent(QCloseEvent *ev);

  Ui::SelectPeakDialog *ui;
  long m_peakWindow;
  int m_selectedPeakNumber;

signals:
  void closedSignal();
  void listClicked(const QModelIndex &index, const QAbstractItemModel *model, const long peakWindow);

private slots:
  void onCancelClicked();
  void onListClicked(const QModelIndex &index);
  void onListDoubleClicked(const QModelIndex &index);
  void onSelectClicked();
};

#endif // SELECTPEAKDIALOG_H
