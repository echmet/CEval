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
  void setPeakWindow(const long peakWindow);

private:
  void closeEvent(QCloseEvent *ev);

  Ui::SelectPeakDialog *ui;
  long m_peakWindow;
  SelectionMode m_selectionMode;
  QVector<int> m_selectedPeakNumbers;

signals:
  void closedSignal();
  void listClicked(const QModelIndex &index, const QAbstractItemModel *model, const long peakWindow);

private slots:
  void onCancelClicked();
  void onListDoubleClicked(const QModelIndex &index);
  void onSelectClicked();
  void onSelectAllClicked();
};

#endif // SELECTPEAKDIALOG_H
