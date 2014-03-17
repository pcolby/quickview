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

QString FileHandlerInfo::defaultIcon(const QString &extension) {
    /*// Use Window's default icon for the given file type.
    const QSettings settings(HKEY_CLASSES_ROOT, QSettings::NativeFormat);

    QString icon1 = settings.value(QString::fromLatin1("%1file/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon.isEmpty()) QMessageBox::information(0, QString::fromLatin1("A: %1").arg(extension), icon);
    if (!icon1.isEmpty()) return icon1;

    QString icon2 = settings.value(QString::fromLatin1("%1Image.Document/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon.isEmpty()) QMessageBox::information(0, QString::fromLatin1("B: %1").arg(extension), icon);
    if (!icon2.isEmpty()) return icon2;

    QString icon3 = settings.value(QString::fromLatin1("PhotoViewer.FileAssoc.%1/DefaultIcon/Default").arg(extension)).toString();
    //if (!icon.isEmpty()) QMessageBox::information(0, QString::fromLatin1("C: %1").arg(extension), icon);
    if (!icon3.isEmpty()) return icon3;

    QMessageBox::information(0, QString::fromLatin1("%1").arg(extension), QString::fromLatin1("A: %1\nB: %2\nC: %3").arg(icon1, icon2, icon3));

    //QMessageBox::information(0, QString::fromLatin1("D: %1").arg(extension), QLatin1String("not found"));*/

    // If Windows has no default, pick our own from Windows's imageres.dll.
    return QString::fromLatin1("%SystemRoot%\\System32\\imageres.dll,%1").arg(defaultIconIndex(extension));
}

int FileHandlerInfo::defaultIconIndex(const QString &extension) {
    if ((extension == QLatin1String("bmp")) || (extension == QLatin1String("ico")) || (extension == QLatin1String("pbm")) ||
        (extension == QLatin1String("pgm")) || (extension == QLatin1String("ppm")) || (extension == QLatin1String("tga")) ||
        (extension == QLatin1String("xbm")) || (extension == QLatin1String("xpm")))
        return -70;
    else if (extension == QLatin1String("gif"))
        return -71;
    else if ((extension == QLatin1String("jpeg")) || (extension == QLatin1String("jpg")))
        return -72;
    else if ((extension == QLatin1String("mng")) || (extension == QLatin1String("png")))
        return -83;
    else if ((extension == QLatin1String("tif")) || (extension == QLatin1String("tiff")))
        return -122;

    /*
    svg: iexplore,-17|  |
    svgz:*/

    QMessageBox messageBox;
    messageBox.setTextFormat(Qt::RichText);
    messageBox.setText(QString::fromLatin1("Unknown file format: %1<br/><br/>Please report this at <a href=\"%2\">%2</a>")
                      .arg(extension).arg(QLatin1String("https://github.com/pcolby/quickview/issues")));
    messageBox.exec();
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
