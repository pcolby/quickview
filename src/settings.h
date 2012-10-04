#include <QString>

namespace Setting {

#define DEFINE_SETTING_NAME(variable, name) static const QString variable(QLatin1String(name))

DEFINE_SETTING_NAME(ExplicitScale,     "scale");
DEFINE_SETTING_NAME(PathName,          "pathname");
DEFINE_SETTING_NAME(SlideShowDuration, "duration");
DEFINE_SETTING_NAME(ZoomMode,          "zoom");

#undef DEFINE_SETTING_NAME

}
