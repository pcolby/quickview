#include "filehandlerinfo.h"
#include "versioninfo.h"
#include <QApplication>
#include <QDir>
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

// Note, this will not override values set in
// Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/.%1/UserChoice/Progid, no will Windows
// let us do so.
bool FileHandlerInfo::enableOpenWith(const QString &extension, const UserScope scope) {
    const QString programId(FileHandlerInfo::programId(extension));
    QSettings settings((scope == AllUsers) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, QSettings::NativeFormat);

    // Setup our program ID class.
    const QString nativeFileName(QDir::toNativeSeparators(QApplication::applicationFilePath()));
    settings.beginGroup(QString::fromLatin1("SOFTWARE/Classes/%1").arg(programId));
    settings.setValue(QLatin1String("DefaultIcon/Default"), QString::fromLatin1("%1,0").arg(nativeFileName));
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
