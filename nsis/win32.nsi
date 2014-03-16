!define QTDIR "C:\Qt\5.2.0\msvc2012"
!define VCRDIR "C:\Windows\System32"

# Include the NSIS Modern UI 2.
!include "MUI2.nsh"

# Installer Attributes: General Attributes.
InstallDir "$PROGRAMFILES\QuickView"
Name "QuickView"
OutFile QuickView-0.1.0.0.exe
RequestExecutionLevel highest # Required for Windows Vista+
SetCompressor lzma
XPStyle on

# Variables.
Var StartMenuFolder

# Modern UI2 Interface Configuration.
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_ABORTWARNING

# Modern UI2 Install Pages.
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl-3.0.rtf"
!insertmacro MUI_PAGE_DIRECTORY

#!insertmacro MUI_PAGE_COMPONENTS
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Paul Colby\QuickView"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "startMenuFolder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
  
# Modern UI2 Uninstall Pages.
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Modern UI2 Languages.
!insertmacro MUI_LANGUAGE "English"

# Sections to install... just one section for now.
Section "QuickView"
  # Files to install.
  SetOutPath $INSTDIR
  File "${VCRDIR}\msvcp110.dll"
  File "${VCRDIR}\msvcr110.dll"
  File "${QTDIR}\bin\icu*51.dll"
  File "${QTDIR}\bin\libEGL.dll"
  File "${QTDIR}\bin\libGLESv2.dll"
  File "${QTDIR}\bin\Qt5Core.dll"
  File "${QTDIR}\bin\Qt5Gui.dll"
  File "${QTDIR}\bin\Qt5Widgets.dll"
  File "..\release\QuickView.exe"
  SetOutPath $INSTDIR\imageformats
  File /x "*d?.dll" "${QTDIR}\plugins\imageformats\*.dll"
  SetOutPath $INSTDIR\platforms
  File /x "*d.dll" "${QTDIR}\plugins\platforms\*.dll"
  WriteRegStr HKCU "Software\Software\Paul Colby\QuickView" "" $INSTDIR
  WriteUninstaller $INSTDIR\Uninstall.exe
  # The various shortcuts.
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QuickView.lnk" "$INSTDIR\QuickView.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$DESKTOP\QuickView.lnk" "$INSTDIR\QuickView.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
  # Windows' add/remove programs information.
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "DisplayName" "QuickView 0.1.0.0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "DisplayIcon" "$\"$INSTDIR\QuickView.exe$\",0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "DisplayVersion" "0.1.0.0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "Publisher" "Paul Colby"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "URLInfoAbout" "https://github.com/pcolby/quickview"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
  WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "NoModify" 1
  WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView" "NoRepair" 1
SectionEnd

# Sections to uninstall.
Section "uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuickView"
  Delete "$DESKTOP\QuickView.lnk"
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QuickView.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  Delete $INSTDIR\msvcp110.dll
  Delete $INSTDIR\msvcr110.dll
  Delete $INSTDIR\icu*51.dll
  Delete $INSTDIR\libEGL.dll
  Delete $INSTDIR\libGLESv2.dll
  Delete $INSTDIR\Qt5Core.dll
  Delete $INSTDIR\Qt5Gui.dll
  Delete $INSTDIR\Qt5Widgets.dll
  Delete $INSTDIR\QuickView.exe
  Delete $INSTDIR\Uninstall.exe
  Delete $INSTDIR\imageformats\*
  Delete $INSTDIR\platforms\*  
  RMDir $INSTDIR\imageformats
  RMDir $INSTDIR\platforms
  RMDir $INSTDIR
  DeleteRegKey /ifempty HKCU "Software\Paul Colby\QuickView"
  DeleteRegKey /ifempty HKCU "Software\Paul Colby"
SectionEnd

# Installer Attributes: Version Information.
VIProductVersion "0.1.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "QuickView"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "https://github.com/pcolby/quickview"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Paul Colby"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "2009-2014 Paul Colby"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "QuickView installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.1.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "0.1.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" "QuickView"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "QuickView-0.1.0.0.exe"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
#VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""