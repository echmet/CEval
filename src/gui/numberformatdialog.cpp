#include "numberformatdialog.h"
#include "ui_numberformatdialog.h"
#include "../doubletostringconvertor.h"
#include <algorithm>

NumberFormatDialog::NumberFormatDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NumberFormatDialog)
{
  ui->setupUi(this);

  m_intValidator.setBottom(1);
  m_intValidator.setTop(16);

  /* Fill list of available locales */
  {
    QString currentLocName = DoubleToStringConvertor::locale().name();
    QString currentLocBcp47Name = DoubleToStringConvertor::locale().bcp47Name();

    int locIdx = 0;
    int ctr = 0;
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::Language::AnyLanguage, QLocale::Script::AnyScript, QLocale::Country::AnyCountry);

    std::sort(allLocales.begin(), allLocales.end(), [](const QLocale &first, const QLocale &second) {
      auto removeDiacritics = [](const QString &string) {
        static const QString diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆČÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝŘŇßàáâãäåæčçèéêëìíîïðñòóôõöøùúûüýÿřň");
        static const QStringList noDiacriticLetters = QStringList() <<"S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<
                                                                      "C"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<
                                                                      "O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"R"<<"N"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"č"<<"c"<<
                                                                      "e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<
                                                                      "u"<<"u"<<"u"<<"y"<<"y"<<"r"<<"n";
        static const int dlLength = diacriticLetters.length();
        Q_ASSERT(diacriticLetters.length() == noDiacriticLetters.length());

        QString newString;

        for (const QChar &ch : string) {
          QString cnvCh = QString(ch);

          for (int idx = 0; idx < dlLength; idx++) {
            const QChar &diaCh = diacriticLetters.at(idx);

            if (ch == diaCh) {
              cnvCh = noDiacriticLetters.at(idx);
              break;
            }
          }
          newString += cnvCh;

        }

        return newString;
      };

      const QString firstName = removeDiacritics(first.nativeTerritoryName());
      const QString secondName = removeDiacritics(second.nativeTerritoryName());

      return firstName < secondName;
    });

    bool isLocIdxSet = false;
    for (const QLocale &loc : allLocales) {
      ui->qcbox_formatting->addItem(QString("%1 (%2)").arg(loc.nativeTerritoryName()).arg(loc.bcp47Name()), loc.name());

      if (loc.name() == currentLocName && loc.bcp47Name() == currentLocBcp47Name && !isLocIdxSet) {
        locIdx = ctr;
        isLocIdxSet = true;
      }
      ctr++;
    }
    ui->qcbox_formatting->setCurrentIndex(locIdx);
  }

  connect(ui->qpb_cancel, &QPushButton::clicked, this, &NumberFormatDialog::onCancelClicked);
  connect(ui->qpb_ok, &QPushButton::clicked, this, &NumberFormatDialog::onOkClicked);
}

NumberFormatDialog::~NumberFormatDialog()
{
  delete ui;
}

int NumberFormatDialog::exec()
{
  const QVariant v = ui->qcbox_formatting->currentData();

  if (!v.isValid())
    return QDialog::exec();

  const QString s = v.toString();
  const QString locStr = DoubleToStringConvertor::locale().name();

  if (s == locStr)
    return QDialog::exec();

  const QAbstractItemModel *model = ui->qcbox_formatting->model();
  for (int idx = 0; idx < model->rowCount(); idx++) {
    const QString is = model->data(model->index(idx, 0), Qt::UserRole).toString();

    if (is == locStr) {
      ui->qcbox_formatting->setCurrentIndex(idx);
      break;
    }
  }

  return QDialog::exec();
}

void NumberFormatDialog::onCancelClicked()
{
  reject();
}

void NumberFormatDialog::onOkClicked()
{
  char type;
  int digits;
  QString locName;
  bool ok;

  if (ui->qcb_trailingZeros->checkState() == Qt::Checked)
    type = 'f';
  else
    type = 'g';

  digits = ui->qle_digits->text().toInt(&ok);
  if (!ok)
    digits = 5;

  locName = ui->qcbox_formatting->currentData().toString();

  DoubleToStringConvertor::setParameters(type, digits, locName);

  accept();
}

void NumberFormatDialog::setParameters(const char type, int digits)
{
  if (type == 'f')
    ui->qcb_trailingZeros->setCheckState(Qt::Checked);
  else
    ui->qcb_trailingZeros->setCheckState(Qt::Unchecked);

  if (digits > 0 && digits < 17)
    ui->qle_digits->setText(QString::number(digits));
  else
    ui->qle_digits->setText("5");
}
