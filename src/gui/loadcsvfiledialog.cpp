#include "loadcsvfiledialog.h"
#include "ui_loadcsvfiledialog.h"

const QString LoadCsvFileDialog::s_qlDelimiterToolTip = tr("Character that separates columns in the data file");
const QString LoadCsvFileDialog::s_qlDecimalSeparatorToolTip = tr("Character that separates integer and decimal parf on a number in the data file");
const QString LoadCsvFileDialog::s_qlFirstLineIsHeaderToolTip = tr("Check when first line in the data file contains captions of the columns");

LoadCsvFileDialog::Parameters::Parameters() :
  delimiter(""),
  xCaption(""),
  yCaption(""),
  hasHeader(false)
{
}

LoadCsvFileDialog::Parameters::Parameters(const QString &delimiter, const QChar &decimalSeparator,
                                          const QString &xCaption, const QString &yCaption, const bool hasHeader) :
  delimiter(delimiter),
  decimalSeparator(decimalSeparator),
  xCaption(xCaption),
  yCaption(yCaption),
  hasHeader(hasHeader)
{
}

LoadCsvFileDialog::Parameters &LoadCsvFileDialog::Parameters::operator=(const Parameters &other)
{
  const_cast<QString&>(delimiter) = other.delimiter;
  const_cast<QChar&>(decimalSeparator) = other.decimalSeparator;
  const_cast<QString&>(xCaption) = other.xCaption;
  const_cast<QString&>(yCaption) = other.yCaption;
  const_cast<bool&>(hasHeader) = other.hasHeader;

  return *this;
}

LoadCsvFileDialog::LoadCsvFileDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LoadCsvFileDialog)
{
  ui->setupUi(this);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &LoadCsvFileDialog::onCancelClicked);
  connect(ui->qcb_hasHeader, &QCheckBox::clicked, this, &LoadCsvFileDialog::onHasHeaderClicked);
  connect(ui->qpb_load, &QPushButton::clicked, this, &LoadCsvFileDialog::onLoadClicked);

  ui->qcbox_decimalSeparator->addItem("Period (.)", QChar('.'));
  ui->qcbox_decimalSeparator->addItem("Comma (,)", QChar(','));

  ui->ql_delimiter->setToolTip(s_qlDelimiterToolTip);
  ui->ql_decimalSeparator->setToolTip(s_qlDecimalSeparatorToolTip);
  ui->ql_hasHeader->setToolTip(s_qlFirstLineIsHeaderToolTip);
}

LoadCsvFileDialog::~LoadCsvFileDialog()
{
  delete ui;
}

void LoadCsvFileDialog::onCancelClicked()
{
  reject();
}

void LoadCsvFileDialog::onHasHeaderClicked()
{
  bool readOnly = ui->qcb_hasHeader->checkState() == Qt::Checked;

  ui->qle_xCaption->setReadOnly(readOnly);
  ui->qle_yCaption->setReadOnly(readOnly);
}

void LoadCsvFileDialog::onLoadClicked()
{
  m_parameters = Parameters(ui->qle_delimiter->text(),
                            ui->qcbox_decimalSeparator->currentData().toChar(),
                            ui->qle_xCaption->text(), ui->qle_yCaption->text(),
                            ui->qcb_hasHeader->checkState() == Qt::Checked);

  accept();
}

LoadCsvFileDialog::Parameters LoadCsvFileDialog::parameters() const
{
  return m_parameters;
}
