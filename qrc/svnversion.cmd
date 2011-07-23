@echo off

:: Make sure the SlikSvn and UnxUtils binaries are in the path.
PATH=%PATH%;C:\Program Files\SlikSvn\bin;C:\Program Files\UnxUtils\usr\local\wbin;C:\Program Files (x86)\UnxUtils\usr\local\wbin

:: Check that we have the right number of commandline arguments.
if "%1" == "" goto usage
if "%2" == "" goto usage
if "%3" == "" goto usage
goto main
:usage
echo usage: %0 svn_working_directory source_file destination_file
pause
goto :EOF

:main
:: Set the REVISION variable to the current subversion revision number.
svnversion.exe %1 | sed.exe -e "s/\([0-9]*:\)\?\([0-9]*\)\([MS]\)\?/\2/" > .revision
set /p REVISION=<.revision
del .revision

::rem Apply the revision number to a *copy* of the source file.
sed.exe -e "s/\(\([ 0-9]\+[.,]\)\{3\} *\)0/\1%REVISION%/g" "%2" > "%2.tmp"

::rem If the revision numbers differ, then overwrite the destination.
diff.exe -qs "%2.tmp" "%3"
IF ERRORLEVEL 1 copy "%2.tmp" "%3"
del "%2.tmp"