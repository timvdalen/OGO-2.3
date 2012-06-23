;-------------------------------------------------------------------------------

Name "Grudge of the Oblivious -- "
OutFile "InstallGoto.exe"
InstallDir $PROGRAMFILES\GOTO
AutoCloseWindow true

SetCompressor /SOLID lzma

;-------------------------------------------------------------------------------
;                             Pages

PageEx license
	LicenseText "Readme" "Next"
	LicenseData "..\bin\README"
PageExEnd

PageEx license
	LicenseText "Game License"
	LicenseData "..\bin\LICENSE"
PageExEnd

Page directory
Page components
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

;-------------------------------------------------------------------------------
;                             Default

Section "Game files"

SectionIn RO

SetOutPath $INSTDIR

File /r ..\bin\*.*

WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GOTOGame" "DisplayName" "Grudge of the Oblivious (remove only)"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GOTOGame" "UninstallString" "$INSTDIR\UninstallGoto.exe"
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GOTOGame" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GOTOGame" "NoRepair" 1
WriteUninstaller "$INSTDIR\UninstallGoto.exe"

ExecWait "$INSTDIR\Setup.exe"
MessageBox MB_OK "GOTO has been installed successfully!"

SectionEnd

;-------------------------------------------------------------------------------
;                             Menu shortcuts

Section "Startmenu shortcuts"

CreateDirectory "$SMPROGRAMS\Goto"
CreateShortcut "$SMPROGRAMS\Goto\Play.lnk" "$INSTDIR\Game.exe"
CreateShortcut "$SMPROGRAMS\Goto\Play (fullscreen).lnk" "$INSTDIR\Game.exe" "--fullscreen"
CreateShortcut "$SMPROGRAMS\Goto\Setup.lnk" "$INSTDIR\Setup.exe"
CreateShortcut "$SMPROGRAMS\Goto\Uninstall.lnk" "$INSTDIR\UninstallGoto.exe"

SectionEnd

;-------------------------------------------------------------------------------
;                             Desktop shortcut

Section "Desktop shortcut"

CreateShortcut "$DESKTOP\GOTO.lnk" "$INSTDIR\Game.exe"

SectionEnd

;-------------------------------------------------------------------------------

Section "Uninstall"

RMDir /r $INSTDIR

Delete "$SMPROGRAMS\Goto\*.*"
RMDir "$SMPROGRAMS\Goto"
Delete "$DESKTOP\GOTO.lnk"

DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\GOTOGame"
DeleteRegKey HKLM "SOFTWARE\GOTOGame"

SectionEnd

;-------------------------------------------------------------------------------