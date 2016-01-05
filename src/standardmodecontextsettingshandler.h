#ifndef STANDARDMODECONTEXTSETTINGSHANDLER_H
#define STANDARDMODECONTEXTSETTINGSHANDLER_H

#include "custommetatypes.h"
#include "modecontextlimited.h"

class StandardModeContextSettingsHandler
{
public:
  StandardModeContextSettingsHandler() = delete;

  static void loadUserSettings(const QVariant &settings, ModeContextLimited &ctx);
  static EMT::StringVariantMap saveUserSettings(ModeContextLimited &ctx, int plotCount);

private:
  static const QString LINE_COLOR_SETTINGS_TAG;
  static const QString LINE_THICKNESS_SETTINGS_TAG;
  static const QString POINT_COLOR_SETTINGS_TAG;
  static const QString POINT_SIZE_SETTINGS_TAG;
  static const QString POINT_STYLE_SETTINGS_TAG;

  static const QString AXIS_FONT_SETTINGS_TAG;

  static const QString AXIS_X_BOTTOM_SETTINGS_TAG;
  static const QString AXIS_Y_LEFT_SETTINGS_TAG;

  static const QString SERIES_SETTINGS_TAG;
  static const QString AXES_SETTINGS_TAG;
};

#endif // STANDARDMODECONTEXTSETTINGSHANDLER_H
