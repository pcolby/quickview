#include "versioninfo.h"

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
#include <windows.h>

// This function is pure win32 code.
bool VersionInfo::getAppVersion(qint16 &major, qint16 &minor, qint16 &error, qint16 &build) {
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
    if (VerQueryValue(VerInfo,L"\\",(LPVOID *)&Ver,&VerLen)) {
            major=HIWORD(Ver->dwFileVersionMS);
            minor=LOWORD(Ver->dwFileVersionMS);
            error=HIWORD(Ver->dwFileVersionLS);
            build=LOWORD(Ver->dwFileVersionLS);
    }

    // Clean up and return.
    HeapFree(GetProcessHeap(),0,VerInfo);
    return true;
}

QString VersionInfo::getAppVersionStr() {
    qint16 major, minor, error, build;
    return (getAppVersion(major,minor,error,build)) ?
            QString::fromAscii("%1.%2.%3.%4").arg(major).arg(minor).arg(error).arg(build) : QString();
}

#endif // Q_OS_WIN
