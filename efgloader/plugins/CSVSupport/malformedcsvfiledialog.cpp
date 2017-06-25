#include "malformedcsvfiledialog.h"
#include "ui_malformedcsvfiledialog.h"

const QString MalformedCsvFileDialog::POSSIBLY_INCORRECT_SETTINGS_MSG = tr("The selected file does not appear to have the desired format.\n"
                                                                           "Check that delimiter and decimal separator are set correctly");
const QString MalformedCsvFileDialog::BAD_DELIMITER_MSG = tr("Invalid delimiter on line %1. Data will be incomplete");
const QString MalformedCsvFileDialog::BAD_TIME_MSG = tr("Invalid value for \"time\" on line %1. Data will be incomplete");
const QString MalformedCsvFileDialog::BAD_VALUE_MSG = tr("Invalid value for \"value\" on line %1. Data will be incomplete");

MalformedCsvFileDialog::MalformedCsvFileDialog(const Error err, const int lineNo, const QString &badLine, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::MalformedCsvFileDialog)
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

  connect(ui->qpb_ok, &QPushButton::clicked, this, &MalformedCsvFileDialog::onOkClicked);
}

MalformedCsvFileDialog::~MalformedCsvFileDialog()
{
  delete ui;
}

void MalformedCsvFileDialog::onOkClicked()
{
  accept();
}
