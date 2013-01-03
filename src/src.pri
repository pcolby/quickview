DEPENDPATH += src
INCLUDEPATH += src
VPATH += $$PWD
SOURCES += main.cpp
HEADERS += settings.h
include(gui/gui.pri)
include(os/os.pri)
