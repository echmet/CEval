#include "appendoverwriteexportfilemessagebox.h"
#include <QPushButton>

AppendOverwriteExportFileMessageBox::AppendOverwriteExportFileMessageBox(QWidget *parent) :
  QMessageBox(parent)
{
  qpb_append = new QPushButton("Append", this);
  qpb_overwrite = new QPushButton("Overwrite", this);

  addButton(qpb_append, QMessageBox::YesRole);
  addButton(qpb_overwrite, QMessageBox::YesRole);
  addButton("Cancel", QMessageBox::NoRole);

  setWindowTitle(tr("File exits"));
  setText(tr("File already exists. Would you like to append new data to the file or overwrite it?"));
  setIcon(QMessageBox::Question);
}

AppendOverwriteExportFileMessageBox::Action AppendOverwriteExportFileMessageBox::action() const
{
  const QAbstractButton *btn = clickedButton();

  if (btn == qpb_append)
    return Action::APPEND;
  else if (btn == qpb_overwrite)
    return Action::OVERWRITE;
  else
    return Action::INVALID;
}
