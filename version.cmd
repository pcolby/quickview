@echo off

:: Make sure the UnxUtils binaries are in the path.
PATH=%PATH%;C:\Program Files\UnxUtils\usr\local\wbin;C:\Program Files (x86)\UnxUtils\usr\local\wbin

:: Check that we have the right number of commandline arguments.
if "%1" == "" goto usage
if "%2" == "" goto usage
goto main
:usage
echo usage: %0 a.b.c d.e.f
pause
goto :EOF

:updateFile
:: Define our updateFile function.
echo Updating "%~3" from %~1 to %~2.
:: Matches "a.b.c"  "a,b,c"  "a, b, c".
echo %~1| sed -e "s/\./\\(\\.\\|\\(, \\?\\)\\)/g" | tr -d "\r\n" > .fromVer
set /p FROM_VER=<.fromVer
echo %~2| sed -e "s/\./\\1/g" | tr -d "\r\n" > .toVer
set /p TO_VER=<.toVer
sed -e "s/%FROM_VER%/%TO_VER%/g" "%~3" > "%~3.tmp" && move "%~3.tmp" "%~3"
del .fromVer .toVer
goto :EOF

:main
:: Process all relevant files.
call:updateFile "%1" "%2" "nsis\win32.nsi"
call:updateFile "%1" "%2" "qrc\SlideShow.rc0"
