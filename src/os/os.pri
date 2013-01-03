DEPENDPATH += src/os
VPATH += $$PWD
win32: HEADERS += filehandlerinfo.h   versioninfo.h
win32: SOURCES += filehandlerinfo.cpp versioninfo.cpp
