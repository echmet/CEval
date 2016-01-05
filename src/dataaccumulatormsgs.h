#ifndef DATAACCUMULATORMSGS_H
#define DATAACCUMULATORMSGS_H

#include <QObject>

class DataAccumulatorMsgs : public QObject {
  Q_OBJECT
public:
  DataAccumulatorMsgs() = delete;

  enum class SeriesType : int {
    MAIN_SIGNAL
  };
  Q_ENUM(SeriesType)

};

#endif // DATAACCUMULATORMSGS_H

