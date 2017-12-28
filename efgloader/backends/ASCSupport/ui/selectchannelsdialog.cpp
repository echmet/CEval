#include "selectchannelsdialog.h"
#include "ui_selectchannelsdialog.h"

#include <QDialogButtonBox>
#include <QCheckBox>
#include <QGridLayout>

SelectChannelsDialog::SelectChannelsDialog(const std::vector<std::string> &channels, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::SelectChannelsDialog}
{
  ui->setupUi(this);

  QGridLayout *lay = static_cast<QGridLayout *>(ui->gridLayout);

  lay->addWidget(new QLabel{"Channel unit", this}, 0, 0);
  lay->addWidget(new QLabel{"Selected",  this}, 0, 1);

  for (size_t idx = 0; idx < channels.size(); idx++) {
    const auto &s = channels.at(idx);
    QLabel *l = new QLabel{this};
    QCheckBox *cb = new QCheckBox{this};
    m_selected.emplace_back(s, cb);

    l->setText(QString::fromStdString(s));
    cb->setChecked(true);
    cb->setText(tr("Selected"));

    lay->addWidget(l, idx + 1, 0);
    lay->addWidget(cb, idx + 1, 1);
  }

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectChannelsDialog::finish);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectChannelsDialog::finish);
  connect(ui->qpb_deselectAll, &QPushButton::clicked, this, &SelectChannelsDialog::onDeselectAll);
  connect(ui->qpb_selectAll, &QPushButton::clicked, this, &SelectChannelsDialog::onSelectAll);
}

SelectChannelsDialog::~SelectChannelsDialog()
{
  delete ui;
}

std::vector<std::pair<std::string, bool>> SelectChannelsDialog::selection() const
{
  std::vector<std::pair<std::string, bool>> sel{};

  for (const auto &item : m_selected)
    sel.emplace_back(item.first, item.second->isChecked());

  return sel;
}

void SelectChannelsDialog::finish()
{
  accept();
}

void SelectChannelsDialog::onDeselectAll()
{
  for (auto &item : m_selected)
    item.second->setChecked(false);
}

void SelectChannelsDialog::onSelectAll()
{
  for (auto &item : m_selected)
    item.second->setChecked(true);
}
