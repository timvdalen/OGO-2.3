;-------------------------------------------------------------------------------

!include "TextFunc.nsh"
!insertmacro ConfigRead
!insertmacro ConfigWrite

Name "Grudge of the Oblivious -- "
OutFile "..\bin\Setup.exe"
!define CONF game.conf

ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"
ReserveFile "config.ini"

Page custom ShowConfig
Page instfiles

InstallButtonText Save

;-------------------------------------------------------------------------------

Function ShowConfig

	${ConfigRead} ${CONF} "playername = " $R0
	${ConfigRead} ${CONF} "team = " $R1
	${ConfigRead} ${CONF} "motd = " $R2
	
	WriteINIStr "$PLUGINSDIR\config.ini" "Field 2" "State" $R0
	
	StrCmp $R1 "a" red
	WriteINIStr "$PLUGINSDIR\config.ini" "Field 5" "State" 1
	Goto done
red:
	WriteINIStr "$PLUGINSDIR\config.ini" "Field 4" "State" 1
done:

	WriteINIStr "$PLUGINSDIR\config.ini" "Field 7" "State" $R2
	
	InstallOptions::dialog "$PLUGINSDIR\config.ini"
	Pop $R0

FunctionEnd

;-------------------------------------------------------------------------------

Function .onInit

	InitPluginsDir
	File /oname=$PLUGINSDIR\config.ini "config.ini"

FunctionEnd

;-------------------------------------------------------------------------------

Section ""
	
	ReadINIStr $R0 "$PLUGINSDIR\config.ini" "Field 2" "State"
	${ConfigWrite} ${CONF} "playername = " $R0 $R1
	
	ReadINIStr $R0 "$PLUGINSDIR\config.ini" "Field 4" "State"
	StrCmp $R0 1 "" notred
	${ConfigWrite} ${CONF} "team = " "a" $R0
notred:
	
	ReadINIStr $R0 "$PLUGINSDIR\config.ini" "Field 5" "State"
	StrCmp $R0 1 "" notblue
	${ConfigWrite} ${CONF} "team = " "b" $R0
notblue:
	
	ReadINIStr $R0 "$PLUGINSDIR\config.ini" "Field 7" "State"
	${ConfigWrite} ${CONF} "motd = " $R0 $R1
	
	Quit
SectionEnd

;-------------------------------------------------------------------------------