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
  enum class SelectionMode {
    INVALID,
    MULTIPLE_PEAK,
    ALL_PEAKS
  };

  explicit SelectPeakDialog(QWidget *parent = nullptr);
  ~SelectPeakDialog();
  void bindModel(QAbstractItemModel *model);
  QVector<int> selectedPeaks() const;
  SelectionMode selectionMode() const;
  void setPeakWindow(const int peakWindow);

private:
  void closeEvent(QCloseEvent *ev);

  Ui::SelectPeakDialog *ui;
  int m_peakWindow;
  SelectionMode m_selectionMode;
  QVector<int> m_selectedPeakNumbers;

signals:
  void allPeaksUnselected();
  void closedSignal();
  void peakSelected(const QModelIndex &index, const QAbstractItemModel *model, const int peakWindow);

private slots:
  void onCancelClicked();
  void onListClicked(const QModelIndex &index);
  void onListDoubleClicked(const QModelIndex &index);
  void onSelectClicked();
  void onSelectAllClicked();
};

#endif // SELECTPEAKDIALOG_H
