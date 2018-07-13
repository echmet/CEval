#ifndef SELECTEDIIPATH_H
#define SELECTEDIIPATH_H

#include <QDialog>

namespace Ui {
class SelectEDIIPath;
}

class SelectEDIIPath : public QDialog
{
  Q_OBJECT
public:
  explicit SelectEDIIPath(QWidget *parent = nullptr);
  ~SelectEDIIPath();

  static QString browseToEDII(const QString &browseFrom);

private:
  Ui::SelectEDIIPath *ui;

private slots:
  void onOkClicked();
  void onBrowseClicked();
};

#endif // SELECTEDIIPATH_H
