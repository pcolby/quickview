@echo off

:: Make sure the NSIS and git binaries are in the path.
PATH=%PATH%;C:\Program Files\NSIS;C:\Program Files (x86)\NSIS;C:\Program Files\Git\bin;C:\Program Files (x86)\Git\bin

:: Set the REVISION variable to the current git revision number.
git.exe rev-list --count HEAD > .revision
set /p REVISION=<.revision
del .revision

:: Update the nsi script, and build the installer.
sed.exe -e "s/\(\([ 0-9]\+[.,]\)\{3\} *\)0/\1%REVISION%/g" win32.nsi > win32.tmp.nsi
makensis win32.tmp.nsi
rm win32.tmp.nsi

pause