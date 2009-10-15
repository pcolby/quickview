#ifndef __VERSION_H__
#define __VERSION_H__
#include <QString>

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
class VersionInfo {
  public:
    static bool getAppVersion(qint16 &major, qint16 &minor, qint16 &error, qint16 &build);
    static QString getAppVersionStr();
};
#endif // Q_OS_WIN

#endif // __VERSION_H__
