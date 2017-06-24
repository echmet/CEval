#ifndef STANDARDPLOTCONTEXTSETTINGSHANDLER_H
#define STANDARDPLOTCONTEXTSETTINGSHANDLER_H

#include "custommetatypes.h"
#include "plotcontextlimited.h"

class StandardPlotContextSettingsHandler
{
public:
  StandardPlotContextSettingsHandler() = delete;

  static void loadUserSettings(const QVariant &settings, PlotContextLimited &ctx);
  static EMT::StringVariantMap saveUserSettings(PlotContextLimited &ctx, int plotCount);

private:
  static const QString SERIE_VISIBLE_SETTINGS_TAG;
  static const QString LINE_STYLE_SETTINGS_TAG;
  static const QString LINE_COLOR_SETTINGS_TAG;
  static const QString LINE_THICKNESS_SETTINGS_TAG;
  static const QString POINT_FILLCOLOR_SETTINGS_TAG;
  static const QString POINT_COLOR_SETTINGS_TAG;
  static const QString POINT_LINETHICKNESS_SETTINGS_TAG;
  static const QString POINT_SIZE_SETTINGS_TAG;
  static const QString POINT_STYLE_SETTINGS_TAG;

  static const QString AXIS_FONT_SETTINGS_TAG;

  static const QString AXIS_X_BOTTOM_SETTINGS_TAG;
  static const QString AXIS_Y_LEFT_SETTINGS_TAG;

  static const QString SERIES_SETTINGS_TAG;
  static const QString AXES_SETTINGS_TAG;
};

#endif // STANDARDPLOTCONTEXTSETTINGSHANDLER_H
