#ifndef __FILEHANDLER_H__
#define __FILEHANDLER_H__
#include <QString>

#ifdef Q_OS_WIN // VersionInfo is a Windows-only class.
class FileHandlerInfo {
  public:
    typedef enum {
        NOT_HANDLED = 0,
        HANDLED_BY_DEFAULT,
        HANDLED_WITHIN_OPEN_WITH_LIST,
        HANDLED_WITHIN_OPEN_WITH_PROGIDS
    } HandleModes;

    static HandleModes isHandledByThisApplication(const QString &extension);
    static bool setHandledByThisApplication(const QString &extension, const HandleModes mode = HANDLED_BY_DEFAULT);

  protected:
    static QString programId(const QString &extension);
};
#endif // Q_OS_WIN

#endif // __FILEHANDLER_H__
