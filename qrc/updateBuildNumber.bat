@echo off
rem This batch file updates build numbers to match the current svn revision number.
rem Usage: updateBuildNumber.bat srcWindowsRcFile dstWindowsRcFile

rem Make sure the SlikSvn and UnxUtils binaries are in the path.
PATH=%PATH%;C:\Program Files\SlikSvn\bin;C:\Program Files\UnxUtils\usr\local\wbin

rem Set the REV_NO variable.
svn info | grep "^Revision: " | cut -d" " -f2 > .revNo
set /p REV_NO=<.revNo
del .revNo

rem Apply the new build number to a *copy* of the source file.
sed -e "s/\(\([ 0-9]\+[.,]\)\{3\} *\)0/\1%REV_NO%/g" "%1" > "%1.tmp"

rem If the build numbers differ, then overwrite the destination.
diff -qs "%1.tmp" "%2"
rem The following logic is "if rc >= 1" - that's MS-DOS for you!
IF ERRORLEVEL 1 copy "%1.tmp" "%2"
del "%1.tmp"