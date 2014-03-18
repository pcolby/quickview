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

#include "filehandlerinfo.h"
#include "versioninfo.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

#ifdef Q_OS_WIN // FileHandlerInfo is a Windows-only class.

#define HKEY_CLASSES_ROOT  QLatin1String("HKEY_CLASSES_ROOT")
#define HKEY_CURRENT_USER  QLatin1String("HKEY_CURRENT_USER")
#define HKEY_LOCAL_MACHINE QLatin1String("HKEY_LOCAL_MACHINE")

bool FileHandlerInfo::isOpenWithEnabled(const QString &extension) {
    // Construct a settings object for querying the native HKCR hive.
    const QSettings settings(HKEY_CLASSES_ROOT, QSettings::NativeFormat);

    // Check if this application is in the extension's "open with program IDs" list.
    // Note, according to the MSDN, the OpenWithProgids entries should be REG_NONE values.
    // This means that even if the key is not found, it is likely that QSettings::value
    // will return an "empty" QVariant, so we use the following "magic" value to detect
    // a non-existant registry key.
    #define NOT_FOUND_MAGIC_VALUE QLatin1String("9B2FC684-A679-41FB-92DE-18AE8B581678")
    const QString key(QString::fromLatin1(".%1\\OpenWithProgids\\%2").arg(extension, programId(extension)));
    return (settings.value(key, NOT_FOUND_MAGIC_VALUE) != NOT_FOUND_MAGIC_VALUE);
}

bool FileHandlerInfo::isOpenWithDefault(const QString &extension) {
    if (!isOpenWithEnabled(extension)) return false;

    // Construct a settings object for querying the native HKCR hive.
    const QSettings settings(HKEY_CLASSES_ROOT, QSettings::NativeFormat);

    // Check if this application is the default handler for the extension.
    return (settings.value(QString::fromLatin1(".%1\\Default").arg(extension)) == programId(extension));
}

// Note, this will not override values set in Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/.%1/UserChoice/Progid,
// nor will Windows let us do so.
bool FileHandlerInfo::enableOpenWith(const QString &extension, const UserScope scope) {
    const QString programId(FileHandlerInfo::programId(extension));
    QSettings settings((scope == AllUsers) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, QSettings::NativeFormat);

    // Setup our program ID class.
    const QString nativeFileName(QDir::toNativeSeparators(QApplication::applicationFilePath()));
    settings.beginGroup(QString::fromLatin1("SOFTWARE/Classes/%1").arg(programId));
    settings.setValue(QLatin1String("DefaultIcon/Default"), defaultIcon(extension));//QString::fromLatin1("%1,0").arg(nativeFileName));
    settings.setValue(QLatin1String("shell/open/command/Default"), QString::fromLatin1("\"%1\" \"%2\"").arg(nativeFileName, QLatin1String("%1")));
    settings.endGroup();

    // Add our program ID to the open-with-progIDs list.
    settings.setValue(QString::fromLatin1("SOFTWARE/Classes/.%1/OpenWithProgids/%2").arg(extension, programId), QString());
    return true;
}

bool FileHandlerInfo::setOpenWithDefault(const QString &extension, const UserScope scope) {
    // Make sure this application is open-with enabled.
    if (!enableOpenWith(extension, scope))
        return false;

    // Set our program ID as the default for this extension.
    QSettings settings((scope == AllUsers) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, QSettings::NativeFormat);
    settings.setValue(QString::fromLatin1("SOFTWARE/Classes/.%1/Default").arg(extension), programId(extension));
    return true;
}

void FileHandlerInfo::clearOpenWithDefault(const QString &extension) {
    clearOpenWithDefault(extension, AllUsers);
    clearOpenWithDefault(extension, CurrentUser);
}

void FileHandlerInfo::clearOpenWithDefault(const QString &extension, const UserScope scope) {
    // If our program ID is the default for this extension, clear it.
    QSettings settings((scope == AllUsers) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, QSettings::NativeFormat);
    if (settings.value(QString::fromLatin1("SOFTWARE/Classes/.%1/Default").arg(extension)) == programId(extension))
        settings.remove(QString::fromLatin1("SOFTWARE/Classes/.%1/Default").arg(extension));
}


void FileHandlerInfo::disableOpenWith(const QString &extension) {
    disableOpenWith(extension, AllUsers);
    disableOpenWith(extension, CurrentUser);
}

void FileHandlerInfo::disableOpenWith(const QString &extension, const UserScope scope) {
    const QString programId(FileHandlerInfo::programId(extension));
    QSettings settings((scope == AllUsers) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, QSettings::NativeFormat);

    // Ensure we're not the default open-with handler for this extension.
    clearOpenWithDefault(extension, scope);

    // Remove our program ID from the open-with-progIDs list.
    settings.remove(QString::fromLatin1("SOFTWARE/Classes/.%1/OpenWithProgids/%2").arg(extension, programId));

    // Remove our program ID class.
    settings.remove(QString::fromLatin1("SOFTWARE/Classes/%1").arg(programId));
}

/* Protected methods */

/// @todo  Replacce all of this with our own set of pretty icons one day.
QString FileHandlerInfo::defaultIcon(const QString &extension) {
    // Use Window's default icon for the given file type.
    const QSettings settings(HKEY_CLASSES_ROOT, QSettings::NativeFormat);

    QString icon1 = settings.value(QString::fromLatin1("%1file/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon1.isEmpty()) QMessageBox::information(0, QString::fromLatin1("A: %1").arg(extension), icon1);
    if (!icon1.isEmpty()) return icon1;

    QString icon2 = settings.value(QString::fromLatin1("%1Image.Document/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon2.isEmpty()) QMessageBox::information(0, QString::fromLatin1("B: %1").arg(extension), icon2);
    if (!icon2.isEmpty()) return icon2;

    QString icon3 = settings.value(QString::fromLatin1("PhotoViewer.FileAssoc.%1/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon3.isEmpty()) QMessageBox::information(0, QString::fromLatin1("C: %1").arg(extension), icon3);
    if (!icon3.isEmpty()) return icon3;

    // Icons can be used as their own file icons, naturally.
    if (extension == QLatin1String("ico")) {
        return QLatin1String("%1");
    }

    // If Windows has no default, pick our own from Windows's imageres.dll.
    const int imageresIndex = defaultImageresIconIndex(extension);
    if (imageresIndex != -1) {
        return QString::fromLatin1("%SystemRoot%\\System32\\imageres.dll,%1").arg(imageresIndex);
    }

    // Otherwise try Window's iexpore.exe.
    const int iexploreIndex = defaultIexploreIconIndex(extension);
    if (iexploreIndex != -1) {
        return QString::fromLatin1("%ProgramFiles%\\Internet Explorer\\iexplore.exe,%1").arg(iexploreIndex);
    }

    // We cannot determine a file type icon: let the user know (this should definitely be picked up in
    // development, long before any end user sees it... unless they are messing with Qt's image plugins.
    QMessageBox messageBox;
    messageBox.setTextFormat(Qt::RichText);
    messageBox.setText(QString::fromLatin1("Unknown file format: %1<br/><br/>Please report this at <a href=\"%2\">%2</a>")
                      .arg(extension).arg(QLatin1String("https://github.com/pcolby/quickview/issues")));
    messageBox.exec();
    return QString();
}

/**
 * @brief Get the index of an icon within iexplore.exe for a given file extension.
 *
 * This function is used as a fallback to return the "known" default icon Windows
 * uses for the specified extension.  Normally, this is detected automatically at
 * runtime by the defaultIcon function above, however, in case the end user's
 * registry is messed up, we fall back to this function.
 *
 * Additionally, some versions of Windows do not have all known extensions. For
 * example, some versions of Windows have no default icon for .svgz files, so here
 * we use the same default icon that Windows uses for .svg files for .svgz too.
 *
 * @param extension File extension (without leading '.') to find an icon index for.
 *
 * @return An icon index assumed to be in iexplore.exe, or -1 if not known.
 */
int FileHandlerInfo::defaultIexploreIconIndex(const QString &extension) {
    if ((extension == QLatin1String("svg")) || (extension == QLatin1String("svgz")))
        return -17;

    return -1;
}

/**
 * @brief Get the index of an icon within imageres.dll for a given file extension.
 *
 * This function is used as a fallback to return the "known" default icon Windows
 * uses for the specified extension.  Normally, this is detected automatically at
 * runtime by the defaultIcon function above, however, in case the end user's
 * registry is messed up, we fall back to this function.
 *
 * Additionally, some versions of Windows do not have all known extensions. For
 * example, some versions of Windows have no default icon for .wbmp files, so here
 * we use the same default icon that Windows uses for .bmp files for .wbmp too.
 *
 * @param extension File extension (without leading '.') to find an icon index for.
 *
 * @return An icon index assumed to be in imageres.dll, or -1 if not known.
 */
int FileHandlerInfo::defaultImageresIconIndex(const QString &extension) {
    if ((extension == QLatin1String("bmp")) || (extension == QLatin1String("ico")) || (extension == QLatin1String("pbm")) ||
        (extension == QLatin1String("pgm")) || (extension == QLatin1String("ppm")) || (extension == QLatin1String("tga")) ||
        (extension == QLatin1String("wbmp"))|| (extension == QLatin1String("xbm")) || (extension == QLatin1String("xpm")))
        return -70;
    else if (extension == QLatin1String("gif"))
        return -71;
    else if ((extension == QLatin1String("jpeg")) || (extension == QLatin1String("jpg")))
        return -72;
    else if ((extension == QLatin1String("mng")) || (extension == QLatin1String("png")))
        return -83;
    else if ((extension == QLatin1String("tif")) || (extension == QLatin1String("tiff")))
        return -122;
    return -1;
}

/**
 *  According to the MSDN, this program ID should take the form:
 *    ProductName.extension.versionMajor.versionMinor
 *  @see http://msdn.microsoft.com/en-us/library/bb165967
 */
QString FileHandlerInfo::programId(const QString &extension) {
    quint16 major(0), minor(0);
    VersionInfo::getAppVersion(major,minor);
    return QString::fromLatin1("%1.%2.%3.%4.%5")
                .arg(QApplication::organizationDomain())
                .arg(QApplication::applicationName())
                .arg(extension).arg(major).arg(minor)
                .replace(QRegExp(QLatin1String("[^-_.a-zA-Z0-9")), QString());
}

#endif // Q_OS_WIN
