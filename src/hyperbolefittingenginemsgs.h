#ifndef HYPERBOLEFITTINGENGINEMSGS
#define HYPERBOLEFITTINGENGINEMSGS

#include <QObject>

class HyperboleFittingEngineMsgs : public QObject
{
  Q_OBJECT
public:
  explicit HyperboleFittingEngineMsgs() = delete;

  enum class MarkerType {
    HORIZONTAL_MARKER,
    VERTICAL_A_MARKER,
    VERTICAL_B_MARKER
  };
  Q_ENUM(MarkerType)

  enum class AnalyteId {
      ANALYTE_A,
      ANALYTE_B
  };
  Q_ENUM(AnalyteId)

};

#endif // HYPERBOLEFITTINGENGINEMSGS

