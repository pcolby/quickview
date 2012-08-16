#ifndef __FILEHANDLER_H__
#define __FILEHANDLER_H__
#include <QString>

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
class FileHandlerInfo {
  public:
    typedef enum {
        AllUsers,
        CurrentUser
    } UserScope;

    static bool isOpenWithEnabled(const QString &extension);
    static bool isOpenWithDefault(const QString &extension);
    static bool enableOpenWith(const QString &extension, const UserScope scope = CurrentUser);
    static bool setOpenWithDefault(const QString &extension, const UserScope scope = CurrentUser);

  protected:
    static QString programId(const QString &extension);
};
#endif // Q_OS_WIN

#endif // __FILEHANDLER_H__
