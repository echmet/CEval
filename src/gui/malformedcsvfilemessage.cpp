#include "malformedcsvfilemessage.h"
#include "ui_malformedcsvfilemessage.h"
#include <QTextDocument>

const QString MalformedCsvFileMessage::POSSIBLY_INCORRECT_SETTINGS_MSG = tr("The selected file does not appear to have the desired format.\n"
                                                                                        "Check that delimiter and decimal separator are set correctly");
const QString MalformedCsvFileMessage::BAD_DELIMITER_MSG = tr("Invalid delimiter on line %1. Data will be incomplete");
const QString MalformedCsvFileMessage::BAD_TIME_MSG = tr("Invalid value for \"time\" on line %1. Data will be incomplete");
const QString MalformedCsvFileMessage::BAD_VALUE_MSG = tr("Invalid value for \"value\" on line %1. Data will be incomplete");

MalformedCsvFileMessage::MalformedCsvFileMessage(const Error err, const int lineNo, const QString &badLine, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::MalformedCsvFileMessage)
{
  ui->setupUi(this);

  ui->qpte_badLine->appendPlainText(badLine);

  switch (err) {
  case Error::POSSIBLY_INCORRECT_SETTINGS:
    ui->ql_message->setText(POSSIBLY_INCORRECT_SETTINGS_MSG);
    break;
  case Error::BAD_DELIMITER:
    ui->ql_message->setText(BAD_DELIMITER_MSG.arg(lineNo));
    break;
  case Error::BAD_TIME_DATA:
    ui->ql_message->setText(BAD_TIME_MSG.arg(lineNo));
    break;
  case Error::BAD_VALUE_DATA:
    ui->ql_message->setText(BAD_VALUE_MSG.arg(lineNo));
    break;
  }

  connect(ui->qpb_ok, &QPushButton::clicked, this, &MalformedCsvFileMessage::onOkClicked);
}

MalformedCsvFileMessage::~MalformedCsvFileMessage()
{
  delete ui;
}

void MalformedCsvFileMessage::onOkClicked()
{
  accept();
}
