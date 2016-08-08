#include "loadcsvfiledialog.h"
#include "ui_loadcsvfiledialog.h"
#include <QStandardItem>

const QString LoadCsvFileDialog::s_qlDelimiterToolTip = tr("Character that separates columns in the data file");
const QString LoadCsvFileDialog::s_qlDecimalSeparatorToolTip = tr("Character that separates integer and decimal parf on a number in the data file");
const QString LoadCsvFileDialog::s_qlFirstLineIsHeaderToolTip = tr("Check when first line in the data file contains captions of the columns");

LoadCsvFileDialog::Parameters::Parameters() :
  delimiter(""),
  decimalSeparator('\0'),
  xColumn(0),
  yColumn(0),
  xType(""),
  yType(""),
  xUnit(""),
  yUnit(""),
  header(HeaderHandling::NO_HEADER),
  linesToSkip(0),
  readBom(false),
  encodingId("")
{
}

LoadCsvFileDialog::Parameters::Parameters(const QString &delimiter, const QChar &decimalSeparator,
                                          const int xColumn, const int yColumn,
                                          const QString &xType, const QString &yType, const QString &xUnit, const QString &yUnit,
                                          const HeaderHandling header, const quint32 linesToSkip,
                                          const bool readBom, const QString &encodingId) :
  delimiter(delimiter),
  decimalSeparator(decimalSeparator),
  xColumn(xColumn),
  yColumn(yColumn),
  xType(xType),
  yType(yType),
  xUnit(xUnit),
  yUnit(yUnit),
  header(header),
  linesToSkip(linesToSkip),
  readBom(readBom),
  encodingId(encodingId)
{
}

LoadCsvFileDialog::Parameters &LoadCsvFileDialog::Parameters::operator=(const Parameters &other)
{
  const_cast<QString&>(delimiter) = other.delimiter;
  const_cast<QChar&>(decimalSeparator) = other.decimalSeparator;
  const_cast<int&>(xColumn) = other.xColumn;
  const_cast<int&>(yColumn) = other.yColumn;
  const_cast<QString&>(xType) = other.xType;
  const_cast<QString&>(yType) = other.yType;
  const_cast<QString&>(xUnit) = other.xUnit;
  const_cast<QString&>(yUnit) = other.yUnit;
  const_cast<HeaderHandling&>(header) = other.header;
  const_cast<quint32&>(linesToSkip) = other.linesToSkip;
  const_cast<bool&>(readBom) = other.readBom;
  const_cast<QString&>(encodingId) = other.encodingId;

  return *this;
}

LoadCsvFileDialog::LoadCsvFileDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LoadCsvFileDialog)
{
  ui->setupUi(this);

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &LoadCsvFileDialog::onCancelClicked);
  connect(ui->qpb_load, &QPushButton::clicked, this, &LoadCsvFileDialog::onLoadClicked);

  ui->qcbox_encoding->setModel(&m_encodingsModel);

  ui->qcbox_decimalSeparator->addItem("Period (.)", QChar('.'));
  ui->qcbox_decimalSeparator->addItem("Comma (,)", QChar(','));

  ui->ql_delimiter->setToolTip(s_qlDelimiterToolTip);
  ui->ql_decimalSeparator->setToolTip(s_qlDecimalSeparatorToolTip);

  /* Set defaut state for BOM checkbox */
  {
    bool enable = ui->qcbox_encoding->currentData(Qt::UserRole + 2).toBool();

    ui->qcb_bom->setEnabled(enable);
  }

  /* Add header handling options */
  ui->qcbox_headerHandling->addItem(tr("No header"), QVariant::fromValue<HeaderHandling>(HeaderHandling::NO_HEADER));
  ui->qcbox_headerHandling->addItem(tr("Header contains units"), QVariant::fromValue<HeaderHandling>(HeaderHandling::HEADER_WITH_UNITS));
  ui->qcbox_headerHandling->addItem(tr("Header without units"), QVariant::fromValue<HeaderHandling>(HeaderHandling::HEADER_WITHOUT_UNITS));

  connect(ui->qcbox_encoding, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &LoadCsvFileDialog::onEncodingChanged);
  connect(ui->qcbox_headerHandling, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &LoadCsvFileDialog::onHeaderHandlingChanged);
}

LoadCsvFileDialog::~LoadCsvFileDialog()
{
  delete ui;
}

void LoadCsvFileDialog::addEncoding(const QString &id, const QString &name, const bool canHaveBom)
{
  QStandardItem *item = new QStandardItem(name);

  if (item == nullptr)
    return;

  item->setData(id, Qt::UserRole + 1);
  item->setData(canHaveBom, Qt::UserRole + 2);
  m_encodingsModel.appendRow(item);
}

void LoadCsvFileDialog::onCancelClicked()
{
  reject();
}

void LoadCsvFileDialog::onEncodingChanged(const int idx)
{
  QStandardItem *item = m_encodingsModel.item(idx);

  if (item == nullptr)
    return;

  bool enable = item->data(Qt::UserRole + 2).toBool();
  ui->qcb_bom->setEnabled(enable);
}

void LoadCsvFileDialog::onHeaderHandlingChanged(const int idx)
{
  QVariant v = ui->qcbox_headerHandling->itemData(idx);

  if (!v.canConvert<HeaderHandling>())
    return;

  HeaderHandling h = v.value<HeaderHandling>();

  switch (h) {
  case HeaderHandling::NO_HEADER:
    ui->qle_xType->setEnabled(true);
    ui->qle_yType->setEnabled(true);
    ui->qle_xUnit->setEnabled(true);
    ui->qle_yUnit->setEnabled(true);
    break;
  case HeaderHandling::HEADER_WITH_UNITS:
    ui->qle_xType->setEnabled(false);
    ui->qle_yType->setEnabled(false);
    ui->qle_xUnit->setEnabled(false);
    ui->qle_yUnit->setEnabled(false);
    break;
  case HeaderHandling::HEADER_WITHOUT_UNITS:
    ui->qle_xType->setEnabled(false);
    ui->qle_yType->setEnabled(false);
    ui->qle_xUnit->setEnabled(true);
    ui->qle_yUnit->setEnabled(true);
    break;
  }
}

void LoadCsvFileDialog::onLoadClicked()
{
  quint32 linesToSkip;
  QStandardItem *item = m_encodingsModel.item(ui->qcbox_encoding->currentIndex());
  if (item == nullptr) {
    reject();
    return;
  }

  QVariant v = ui->qcbox_headerHandling->currentData();

  if (!v.canConvert<HeaderHandling>()) {
    reject();
    return;
  }

  HeaderHandling h = v.value<HeaderHandling>();

  linesToSkip = ui->qspbox_linesToSkip->value();

  m_parameters = Parameters(ui->qle_delimiter->text(),
                            ui->qcbox_decimalSeparator->currentData().toChar(),
                            ui->qspbox_xColumn->value(), ui->qspbox_yColumn->value(),
                            ui->qle_xType->text(), ui->qle_yType->text(),
                            ui->qle_xUnit->text(), ui->qle_yUnit->text(),
                            h, linesToSkip,
                            ui->qcb_bom->checkState() == Qt::Checked,
                            item->data(Qt::UserRole + 1).toString());

  accept();
}

LoadCsvFileDialog::Parameters LoadCsvFileDialog::parameters() const
{
  return m_parameters;
}
