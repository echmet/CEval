#include "witchcraft.h"
#include <QSplitterHandle>
#include <QTextStream>
#include <QVBoxLayout>

void Witchcraft::makeSplitterAppear(QSplitter *splitter)
{
#ifdef Q_OS_WIN
  QSplitterHandle* handle = splitter->handle(splitter->count()-1);
  QVBoxLayout* layout = new QVBoxLayout(handle);
  layout->setSpacing(5);
  layout->setMargin(0);

  QFrame* line = new QFrame(handle);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  layout->addWidget(line);
#else
  Q_UNUSED(splitter)
#endif
}
