#include <QString>

#define DEFINE_SETTING_NAME(variable, name) static const QString variable(QLatin1String(name))

DEFINE_SETTING_NAME(PathNameSetting,          "pathname");
DEFINE_SETTING_NAME(SlideShowDurationSetting, "duration");

#undef DEFINE_SETTING_NAME
