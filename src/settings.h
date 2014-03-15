/*
 *  Copyright 2009-2014 Paul Colby
 *
 *  This file is part of QuickView.
 *
 *  QuickView is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  QuickView is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with QuickView.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QString>

namespace Setting {

#define DEFINE_SETTING_NAME(variable, name) static const QString variable(QLatin1String(name))

DEFINE_SETTING_NAME(ExplicitScale,     "scale");
DEFINE_SETTING_NAME(PathName,          "pathname");
DEFINE_SETTING_NAME(SlideShowDuration, "duration");
DEFINE_SETTING_NAME(ZoomMode,          "zoom");

#undef DEFINE_SETTING_NAME

}
