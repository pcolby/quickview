# Include the NSIS Modern UI 2.
!include "MUI2.nsh"

# Installer Attributes: General Attributes.
InstallDir "$PROGRAMFILES\SlideShow"
Name "SlideShow"
OutFile SlideShow-0.0.0.0.exe
RequestExecutionLevel highest # Required for Windows Vista+
XPStyle on

# Variables.
Var StartMenuFolder

# Modern UI2 Interface Configuration.
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp"
!define MUI_ABORTWARNING
#!define MUI_ICON "..\qrc\appIcon.ico"

# Modern UI2 Install Pages.
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY

#!insertmacro MUI_PAGE_COMPONENTS
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Paul Colby\SlideShow"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "startMenuFolder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
  
# Modern UI2 Uninstall Pages.
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Modern UI2 Languages.
!insertmacro MUI_LANGUAGE "English"

# Sections to install... just one section for now.
Section "SlideShow"
  # Files to install.
  SetOutPath $INSTDIR
  File "C:\MinGW\bin\libgcc_s_dw2-1.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"
  File "D:\Qt\4.6.0-beta1\bin\QtCore4.dll"
  File "D:\Qt\4.6.0-beta1\bin\QtGui4.dll"
  File "..\release\SlideShow.exe"
  SetOutPath $INSTDIR\imageformats
  File /x "*d?.dll" "D:\Qt\4.6.0-beta1\plugins\imageformats\*.dll"
  WriteRegStr HKCU "Software\Software\Paul Colby\SlideShow" "" $INSTDIR
  WriteUninstaller $INSTDIR\Uninstall.exe
  # The various shortcuts.
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\SlideShow.lnk" "$INSTDIR\SlideShow.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$DESKTOP\SlideShow.lnk" "$INSTDIR\SlideShow.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
  # Windows' add/remove programs information.
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "DisplayName" "SlideShow 0.0.0.0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "DisplayIcon" "$\"$INSTDIR\slideshow.exe$\",0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "DisplayVersion" "0.0.0.0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "Publisher" "Paul Colby"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "URLInfoAbout" "http://colby.id.au/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
  WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "NoModify" 1
  WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow" "NoRepair" 1
SectionEnd

# Sections to uninstall.
Section "uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SlideShow"
  Delete "$DESKTOP\SlideShow.lnk"
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\SlideShow.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\msvcr90.dll
  Delete $INSTDIR\QtCore4.dll
  Delete $INSTDIR\QtGui4.dll
  Delete $INSTDIR\SlideShow.exe
  Delete $INSTDIR\Uninstall.exe
  Delete $INSTDIR\imageformats\*
  RMDir $INSTDIR\imageformats
  RMDir $INSTDIR
  DeleteRegKey /ifempty HKCU "Software\Paul Colby\SlideShow"
  DeleteRegKey /ifempty HKCU "Software\Paul Colby"
SectionEnd

# Installer Attributes: Version Information.
VIProductVersion "0.0.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "SlideShow"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "..."
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Paul Colby"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "(c) 2009"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "SlideShow installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.0.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "0.0.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" "SlideShow"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "SlideShow-0.0.0.0.exe"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
#VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""