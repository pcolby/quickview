#include "filehandlerinfo.h"
#include "versioninfo.h"
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QVariant>

#ifdef Q_OS_WIN // FileHandlerInfo is a Windows-only class.

FileHandlerInfo::HandleModes FileHandlerInfo::isHandledByThisApplication(const QString &extension) {
    // Construct a settings object for querying the native HKCR hive.
    QSettings settings(QLatin1String("HKEY_CLASSES_ROOT"), QSettings::NativeFormat);

    // Check if this application is the default hanlder for the extension.
    const QString programId(FileHandlerInfo::programId(extension));
    if (settings.value(QString::fromLatin1(".%1\\Default").arg(extension)).toString() == programId)
        return HANDLED_BY_DEFAULT;

    // Check if this application is in the extension's "open with program IDs" list.
    // Note, according to the MSDN, the OpenWithProgids entries should be REG_NONE values.
    // This means that even if the key is not found, it is likely that QSettings::value
    // will return an "empty" QVariant, so we use the following "magic" value to detect
    // a non-existant registry key.
    #define NOT_FOUND_MAGIC_VALUE QLatin1String("9B2FC684-A679-41FB-92DE-18AE8B581678")
    if (settings.value(QString::fromLatin1(".%1\\OpenWithProgids\\%2").arg(extension).arg(programId), NOT_FOUND_MAGIC_VALUE) != NOT_FOUND_MAGIC_VALUE)
        return HANDLED_WITHIN_OPEN_WITH_PROGIDS;

    /// @todo  Check if this application is in the extension's "open with list" list.
    return NOT_HANDLED;
}

bool FileHandlerInfo::setHandledByThisApplication(const QString &extension, const HandleModes mode) {
    Q_ASSERT(mode != NOT_HANDLED);

    // Make sure we have the relevant applicatioin class already.
    const QString programId(FileHandlerInfo::programId(extension));
    /*QSettings settings(QString::fromLatin1("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\%1").arg(programId), QSettings::NativeFormat);
    settings.setValue(QLatin1String("shell/open/command/Default"),
        QString::fromLatin1("\"%1\" \"%2\"").arg(QDir::toNativeSeparators(QApplication::applicationFilePath()), QLatin1String("%1")));*/

    QSettings settings(QString::fromLatin1("HKEY_CURRENT_USER\\SOFTWARE\\Classes"), QSettings::NativeFormat);
    settings.setValue(QString::fromLatin1("Applications/SlideShow.exe/shell/open/command/Default"),
        QString::fromLatin1("\"%1\" \"%2\"").arg(QDir::toNativeSeparators(QApplication::applicationFilePath()), QLatin1String("%1")));
    settings.setValue(QString::fromLatin1("%1/shell/open/command/Default").arg(programId),
        QString::fromLatin1("\"%1\" \"%2\"").arg(QDir::toNativeSeparators(QApplication::applicationFilePath()), QLatin1String("%1")));
    settings.setValue(QString::fromLatin1(".%1/Default").arg(extension), programId);
    settings.setValue(QString::fromLatin1(".%1/OpenWithList/SlideShow.exe/Default").arg(extension), QString());
    settings.setValue(QString::fromLatin1(".%1/OpenWithProgids/%2").arg(extension, programId), QString());

    QSettings settings2(QString::fromLatin1("HKEY_CURRENT_USER"), QSettings::NativeFormat);
    settings2.setValue(QString::fromLatin1("Software/Microsoft/Windows/CurrentVersion/Explorer/FileExts/.%1/UserChoice/Progid").arg(extension), programId);

    // Check if HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ext\UserChoice\Progid is set, and not us!

    return false;
}

// According to the MSDN, this program ID should take the form:
//   ProductName.extension.versionMajor.versionMinor
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
