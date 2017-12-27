#ifndef SELECTCHANNELSDIALOG_H
#define SELECTCHANNELSDIALOG_H

#include <QDialog>
#include <string>
#include <vector>

namespace Ui {
class SelectChannelsDialog;
}

class QCheckBox;

class SelectChannelsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SelectChannelsDialog(const std::vector<std::string> &channels, QWidget *parent = nullptr);
  ~SelectChannelsDialog();
  std::vector<std::pair<std::string, bool>> selection() const;

private:
  Ui::SelectChannelsDialog *ui;

  std::vector<std::pair<std::string, QCheckBox *>> m_selected;

private slots:
  void finish();
  void onDeselectAll();
  void onSelectAll();
};

#endif // SELECTCHANNELSDIALOG_H
