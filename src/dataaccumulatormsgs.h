#ifndef DATAACCUMULATORMSGS_H
#define DATAACCUMULATORMSGS_H

#include <QObject>

class DataAccumulatorMsgs : public QObject {
  Q_OBJECT
public:
  DataAccumulatorMsgs() = delete;

  enum class ExportAction {
    EXPORT_DATATABLE_CSV,
    EXPORT_PLOT,
    WHOLE_PEAK_TO_CLIPBOARD
  };
  Q_ENUM(ExportAction)

  enum class ProgramMode {
    EVALUATION,
    HYPERBOLE_FIT
  };

  Q_ENUM(ProgramMode)
  enum class SeriesType : int {
    MAIN_SIGNAL
  };
  Q_ENUM(SeriesType)

};

#endif // DATAACCUMULATORMSGS_H

