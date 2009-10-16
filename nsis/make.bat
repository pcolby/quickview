@echo off

PATH=%PATH%;C:\Program Files\NSIS;C:\Program Files\SlikSvn\bin;C:\Program Files\UnxUtils\usr\local\wbin

rem Set the REV_NO variable.
svn info | grep "^Revision: " | cut -d" " -f2 > .revNo
set /p REV_NO=<.revNo
del .revNo

rem Update the nsi script.
sed -e "s/\(\([ 0-9]\+[.,]\)\{3\} *\)0/\1%REV_NO%/g" win32.nsi > win32.tmp.nsi
makensis win32.tmp.nsi
rm win32.tmp.nsi

pause