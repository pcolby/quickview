#include "versioninfo.h"

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
#include <windows.h>

// This function is pure win32 code.
bool VersionInfo::getAppVersion(quint16 &major, quint16 &minor, quint16 &error, quint16 &build) {
    // Get the name of this executable.
    WCHAR FileName[MAX_PATH+1];
    if (!GetModuleFileName(NULL,FileName,MAX_PATH)) return false;

    // Get the size of the version info resource.
    DWORD Ignored, VerInfoSize=GetFileVersionInfoSize(FileName,&Ignored);
    if (!VerInfoSize) return false;

    // Get the version info resource.
    LPVOID VerInfo=HeapAlloc(GetProcessHeap(),0,VerInfoSize);
    if (!VerInfo) return false;
    if (!GetFileVersionInfo(FileName,Ignored,VerInfoSize,VerInfo)) {
            HeapFree(GetProcessHeap(),0,VerInfo);
            return false;
    }

    // Get the product version from the version info resource.
    VS_FIXEDFILEINFO *Ver; UINT VerLen;
    // TODO: Remove the non-const LPTSTR cast from the following call when Qt's MinGW's winver.h is updated (not yet in Qt 4.7.3).
    // See https://sourceforge.net/tracker/index.php?func=detail&aid=2724138&group_id=2435&atid=102435
    if (VerQueryValue(VerInfo,(LPTSTR)L"\\",(LPVOID *)&Ver,&VerLen)) {
            major=HIWORD(Ver->dwFileVersionMS);
            minor=LOWORD(Ver->dwFileVersionMS);
            error=HIWORD(Ver->dwFileVersionLS);
            build=LOWORD(Ver->dwFileVersionLS);
    }

    // Clean up and return.
    HeapFree(GetProcessHeap(),0,VerInfo);
    return true;
}

bool VersionInfo::getAppVersion(quint16 &major, quint16 &minor, quint16 &error) {
    quint16 ignored;
    return getAppVersion(major, minor, error, ignored);
}

bool VersionInfo::getAppVersion(quint16 &major, quint16 &minor) {
    quint16 ignored;
    return getAppVersion(major, minor, ignored);
}

QString VersionInfo::getAppVersionStr() {
    quint16 major, minor, error, build;
    return (getAppVersion(major,minor,error,build)) ?
            QString::fromLatin1("%1.%2.%3.%4").arg(major).arg(minor).arg(error).arg(build) : QString();
}

#endif // Q_OS_WIN
