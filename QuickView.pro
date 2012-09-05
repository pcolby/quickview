# Make an application called QuickView
TARGET = QuickView
TEMPLATE = app

# Disable automatic ASCII conversions (best practice, especially for i18n support).
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

# Define DEBUG (C/C++ pre-processor) in debug (qmake target) mode.
CONFIG(debug,debug|release):DEFINES+=DEBUG

# Set the dependancies.
win32:RC_FILE=qrc/QuickView.rc
include(src/src.pri)

# Neaten the output directories.
CONFIG(debug,debug|release) {
  MOC_DIR = debug/tmp
  OBJECTS_DIR = debug/tmp
  RCC_DIR = debug/tmp
}
CONFIG(release,debug|release) {
  MOC_DIR = release/tmp
  OBJECTS_DIR = release/tmp
  RCC_DIR = release/tmp
}

# Include any necessary link options.
win32:LIBS += -lversion

# Optionally, create our custom svnbuild target.
win32:svnbuild.commands = qrc\\svnversion.cmd . qrc\\QuickView.rc0 qrc\\QuickView.rc
QMAKE_EXTRA_TARGETS += svnbuild

# Hook our optional svnbuild target in between qmake's Makefile update and the actual project target.
svnbuildhook.depends = svnbuild
CONFIG(debug,debug|release):svnbuildhook.target = Makefile.Debug
CONFIG(release,debug|release):svnbuildhook.target = Makefile.Release
QMAKE_EXTRA_TARGETS += svnbuildhook

# Enable GNU profiling (if desired).
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
