#ifndef APPENDOVERWRITEEXPORTFILEMESSAGEBOX_H
#define APPENDOVERWRITEEXPORTFILEMESSAGEBOX_H

#include <QMessageBox>

class QAbstractButton;

class AppendOverwriteExportFileMessageBox : public QMessageBox
{
  Q_OBJECT
public:
  enum class Action {
    INVALID,
    APPEND,
    OVERWRITE
  };

  explicit AppendOverwriteExportFileMessageBox(QWidget *parent = nullptr);
  Action action() const;

private:
  QAbstractButton *qpb_append;
  QAbstractButton *qpb_overwrite;

};

#endif // APPENDOVERWRITEEXPORTFILEMESSAGEBOX_H
