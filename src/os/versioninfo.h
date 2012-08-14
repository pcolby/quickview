#ifndef __VERSION_H__
#define __VERSION_H__
#include <QString>

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
class VersionInfo {
  public:
    static bool getAppVersion(quint16 &major, quint16 &minor, quint16 &error, quint16 &build);
    static bool getAppVersion(quint16 &major, quint16 &minor, quint16 &error);
    static bool getAppVersion(quint16 &major, quint16 &minor);
    static QString getAppVersionStr();
};
#endif // Q_OS_WIN

#endif // __VERSION_H__
