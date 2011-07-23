@echo off

:: Make sure the NSIS, SlikSvn and UnxUtils binaries are in the path.
PATH=%PATH%;C:\Program Files\NSIS;C:\Program Files (x86)\NSIS;C:\Program Files\SlikSvn\bin;C:\Program Files\UnxUtils\usr\local\wbin;C:\Program Files (x86)\UnxUtils\usr\local\wbin

:: Set the REVISION variable to the current subversion revision number.
svnversion.exe .. | sed.exe -e "s/\([0-9]*:\)\?\([0-9]*\)\([MS]\)\?/\2/" > .revision
set /p REVISION=<.revision
del .revision

:: Update the nsi script, and build the installer.
sed -e "s/\(\([ 0-9]\+[.,]\)\{3\} *\)0/\1%REVISION%/g" win32.nsi > win32.tmp.nsi
makensis win32.tmp.nsi
rm win32.tmp.nsi

pause