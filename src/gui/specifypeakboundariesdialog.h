#ifndef SPECIFYPEAKBOUNDARIESDIALOG_H
#define SPECIFYPEAKBOUNDARIESDIALOG_H

#include <QDialog>

namespace Ui {
class SpecifyPeakBoundariesDialog;
}

class SpecifyPeakBoundariesDialog : public QDialog
{
  Q_OBJECT
public:
  class Answer {
  public:
    Answer();
    Answer(const double fromX, const double fromY, const double toX, const double toY,
           const bool snapFrom, const bool snapTo);
    Answer(const Answer &other);
    bool isValid() const;
    Answer & operator=(const Answer &other);

    const double fromX;
    const double fromY;
    const double toX;
    const double toY;
    const bool snapFrom;
    const bool snapTo;

  private:
    bool m_valid;
  };

  explicit SpecifyPeakBoundariesDialog(QWidget *parent = nullptr);
  ~SpecifyPeakBoundariesDialog();
  Answer answer();

private:
  Ui::SpecifyPeakBoundariesDialog *ui;
  Answer m_answer;

private slots:
  void onCancelClicked();
  void onOkClicked();
  void onSnapFromChanged(const int state);
  void onSnapToChanged(const int state);

};

#endif // SPECIFYPEAKBOUNDARIESDIALOG_H
