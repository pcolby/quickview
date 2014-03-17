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
Page custom imageAssociationsPage onImageAssociationsPageLeave
!insertmacro MUI_PAGE_INSTFILES
  
# Modern UI2 Uninstall Pages.
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Modern UI2 Languages.
!insertmacro MUI_LANGUAGE "English"

# Custom install pages.

Function imageAssociationsPage
	# Create a "standard" wizard page (as opposed to the welcome and finish pages).
	!insertmacro MUI_HEADER_TEXT "Image File Associations" "Choose whether not to associate QuickView with supported image types."
	nsDialogs::Create 1018
	Pop $0
	${If} $0 == error
		Abort
	${EndIf}

	# Create some explanatory text.
	${NSD_CreateLabel} 0 0 100% 30u "QuickView may be associated with supported image types, making it easier to open images from Explorer, etc.  Please choose which file association options you would like to enable, then click Next to continue."
	Pop $0
	
	# Create the username and password edit boxes.
	Var /GLOBAL hWndOpenWith
	Var /GLOBAL hWndDefaultApp
	${NSD_CreateCheckBox} 0 60u 100% 12u "Associate QuickView with supported image types"
	Pop $hWndOpenWith
	${NSD_OnClick} $hWndOpenWith onOpenWithCheckBoxClick
	${NSD_SetState} $hWndOpenWith ${BST_CHECKED}
	${NSD_CreateCheckBox} 0 71u 100% 12u "Make QuickView the default program for supported image types"
	Pop $hWndDefaultApp
	
	# Show the dialog.
	nsDialogs::Show
FunctionEnd

Function onOpenWithCheckBoxClick
	# Enabled / disable the "Set as default" CheckBox, according to the "Open with" CheckBox.
	Push $R0
	${NSD_GetState} $hWndOpenWith $R0
	${If} $R0 == ${BST_UNCHECKED}
		${NSD_SetState} $hWndDefaultApp ${BST_UNCHECKED}
	    EnableWindow $hWndDefaultApp 0 ; Disble the "Set as default" CheckBox.
	${Else}
	    EnableWindow $hWndDefaultApp 1 ; Ensble the "Set as default" CheckBox.
    ${EndIf}
	Pop $R0
FunctionEnd

Function onImageAssociationsPageLeave
	# Fetch the text from the edit boxes.
    Var /GLOBAL addOpenWith
    Var /GLOBAL setAsDefaultApp	
	${NSD_GetState} $hWndOpenWith $addOpenWith
	${NSD_GetState} $hWndDefaultApp $setAsDefaultApp
FunctionEnd

# Sections to install.

Section "application"
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

Section "file associations"
    ${If} $setAsDefaultApp == ${BST_CHECKED}
        DetailPrint "Setting QuickView as default application for supported image types..."
        Exec '"$INSTDIR\QuickView.exe" -set-open-with-default'
	${ElseIf} $addOpenWith == ${BST_CHECKED}
        DetailPrint "Enabling shell extension(s) for supported image types..."
        Exec '"$INSTDIR\QuickView.exe" -enable-open-with'
	${EndIf}
SectionEnd

# Sections to uninstall.

Section "un.file associations"
    DetailPrint "Removing any registered shell extensions..."
    Exec '"$INSTDIR\QuickView.exe" -remove-open-with'
SectionEnd

Section "un.application"
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