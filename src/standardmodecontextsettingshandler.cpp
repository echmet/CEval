#include "standardmodecontextsettingshandler.h"

const QString StandardModeContextSettingsHandler::LINE_COLOR_SETTINGS_TAG("Line-Color");
const QString StandardModeContextSettingsHandler::LINE_THICKNESS_SETTINGS_TAG("Line-Thickness");
const QString StandardModeContextSettingsHandler::POINT_COLOR_SETTINGS_TAG("Point-Color");
const QString StandardModeContextSettingsHandler::POINT_SIZE_SETTINGS_TAG("Point-Size");
const QString StandardModeContextSettingsHandler::POINT_STYLE_SETTINGS_TAG("Point-Style");

const QString StandardModeContextSettingsHandler::AXIS_FONT_SETTINGS_TAG("Axis-Font");

const QString StandardModeContextSettingsHandler::AXIS_X_BOTTOM_SETTINGS_TAG("Axis-XBottom");
const QString StandardModeContextSettingsHandler::AXIS_Y_LEFT_SETTINGS_TAG("Axis-YLeft");

const QString StandardModeContextSettingsHandler::SERIES_SETTINGS_TAG("Series");
const QString StandardModeContextSettingsHandler::AXES_SETTINGS_TAG("Axes");


void StandardModeContextSettingsHandler::loadUserSettings(const QVariant &settings, ModeContextLimited &ctx)
{
  EMT::StringVariantMap outerMap = settings.value<EMT::StringVariantMap>();

  if (outerMap.contains(SERIES_SETTINGS_TAG)) {
    QVariant v = outerMap[SERIES_SETTINGS_TAG];
    EMT::SeriesSettingsMap map;

    if (v.canConvert<EMT::SeriesSettingsMap>())
      map = v.value<EMT::SeriesSettingsMap>();

    for (const int key : map.keys()) {
      QMap<QString, QVariant> innerMap = map[key];
      SerieProperties::VisualStyle vs;

      if (!ctx.serieVisualStyle(key, vs))
        continue;

      if (innerMap.contains(LINE_COLOR_SETTINGS_TAG)) {
        QVariant v = innerMap[LINE_COLOR_SETTINGS_TAG];

        if (v.canConvert<QColor>()) {
          QColor c = v.value<QColor>();
          vs.pen.setColor(c);
        }
      }

      if (innerMap.contains(LINE_THICKNESS_SETTINGS_TAG)) {
        QVariant v = innerMap[LINE_THICKNESS_SETTINGS_TAG];

        if (v.canConvert<qreal>()) {
          qreal t = v.value<qreal>();
          vs.pen.setWidthF(t);
        }
      }

      if (innerMap.contains(POINT_COLOR_SETTINGS_TAG)) {
        QVariant v = innerMap[POINT_COLOR_SETTINGS_TAG];

        if (v.canConvert<QColor>()) {
          QColor c = v.value<QColor>();
          QPen p(vs.symbol()->pen());
          p.setColor(c);
          vs.symbol()->setPen(p);
        }
      }

      if (innerMap.contains(POINT_SIZE_SETTINGS_TAG)) {
        QVariant v = innerMap[POINT_SIZE_SETTINGS_TAG];

        if (v.canConvert<int>())
          vs.symbol()->setSize(v.toInt());
      }

      if (innerMap.contains(POINT_STYLE_SETTINGS_TAG)) {
        QVariant v = innerMap[POINT_STYLE_SETTINGS_TAG];

        if (v.canConvert<int>())
          vs.symbol()->setStyle(static_cast<QwtSymbol::Style>(v.toInt()));
      }

      ctx.setSerieVisualStyle(key, vs);
    }
  }

  if (outerMap.contains(AXES_SETTINGS_TAG)) {
    QVariant v = outerMap[AXES_SETTINGS_TAG];
    EMT::StringVariantMap map;

    if (v.canConvert<EMT::StringVariantMap>())
      map = v.value<EMT::StringVariantMap>();

    if (map.contains(AXIS_X_BOTTOM_SETTINGS_TAG)) {
      QVariant v = map[AXIS_X_BOTTOM_SETTINGS_TAG];

      if (v.canConvert<QFont>()) {
        QFont f = v.value<QFont>();
        ctx.setAxisFont(SerieProperties::Axis::X_BOTTOM, f);
      }
    }

    if (map.contains(AXIS_Y_LEFT_SETTINGS_TAG)) {
      QVariant v = map[AXIS_Y_LEFT_SETTINGS_TAG];

      if (v.canConvert<QFont>()) {
        QFont f = v.value<QFont>();
        ctx.setAxisFont(SerieProperties::Axis::Y_LEFT, f);
      }
    }
  }

}

EMT::StringVariantMap StandardModeContextSettingsHandler::saveUserSettings(ModeContextLimited &ctx, int plotCount)
{
  EMT::StringVariantMap outerMap;

  EMT::SeriesSettingsMap seriesSettings;
  for (int id = 0; id < plotCount; id++) {
    SerieProperties::VisualStyle vs;

    if (!ctx.serieVisualStyle(id, vs))
      continue;

    QMap<QString, QVariant> map;

    map.insert(LINE_COLOR_SETTINGS_TAG, vs.pen.color());
    map.insert(LINE_THICKNESS_SETTINGS_TAG, vs.pen.widthF());
    map.insert(POINT_COLOR_SETTINGS_TAG, vs.symbol()->pen().color());
    map.insert(POINT_SIZE_SETTINGS_TAG, vs.symbol()->size().width());
    map.insert(POINT_STYLE_SETTINGS_TAG, vs.symbol()->style());

    seriesSettings.insert(id, map);
  }
  outerMap.insert(SERIES_SETTINGS_TAG, QVariant::fromValue<EMT::SeriesSettingsMap>(seriesSettings));

  EMT::StringVariantMap axisSettings;
  axisSettings[AXIS_X_BOTTOM_SETTINGS_TAG] = QVariant(ctx.axisFont(SerieProperties::Axis::X_BOTTOM));
  axisSettings[AXIS_Y_LEFT_SETTINGS_TAG] = QVariant(ctx.axisFont(SerieProperties::Axis::Y_LEFT));

  outerMap.insert(AXES_SETTINGS_TAG, axisSettings);

  return outerMap;
}
