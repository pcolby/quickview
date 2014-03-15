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
    static void clearOpenWithDefault(const QString &extension);
    static void clearOpenWithDefault(const QString &extension, const UserScope scope);
    static void disableOpenWith(const QString &extension);
    static void disableOpenWith(const QString &extension, const UserScope scope);

  protected:
    static QString defaultIcon(const QString &extension);
    static int defaultIconIndex(const QString &extension);
    static QString programId(const QString &extension);
};
#endif // Q_OS_WIN

#endif // __FILEHANDLER_H__
