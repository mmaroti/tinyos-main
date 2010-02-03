!define APPVERSION "0.0.2"  ; Define application version
!define PATH_ROOT "../" ;we are in installer dir
!define INSTALLERVERSION 02 ;
!define SHORTCUTS "ShimmerLogger"
OutFile "shimmerlogger-${APPVERSION}-install.exe"
    SetPluginUnload  alwaysoff
	SetCompressor LZMA
	!define DIR "$PROGRAMFILES\ShimmerLogger\"
    InstallDir "${DIR}"
	InstallDirRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "Install Folder"
	CRCCheck force
			ShowInstDetails show
			ShowUninstDetails show
Section "!ShimmerLogger" Section1
	SetOverwrite off
	; Copy data files
	SetOutPath "$INSTDIR\Adatok\"
	File ${PATH_ROOT}\Adatok\*.csv

	; Overwrite files by default, but don't complain on failure
	SetOverwrite try
	; Copy dll files
	SetOutPath "$INSTDIR\"
	File ${PATH_ROOT}\lib_win\dll\*.dll
	
	; Copy text files
	;SetOutPath "$INSTDIR\"
	;File ${PATH_ROOT}\README_WINDOWS.txt
	;File ${PATH_ROOT}\COPYRIGHT.txt

	; Copy executable
	File /oname=ShimmerLogger.exe ${PATH_ROOT}\release\ShimmerLogger.exe

	; Create the Registry Entries
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "DisplayIcon" "$INSTDIR\ShimmerLogger.exe,0"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "DisplayName" "ShimmerLogger ${APPVERSION}"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "DisplayVersion" "${APPVERSION}"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "Install Folder" "$INSTDIR"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "Publisher" "ShimmerLogger"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "Shortcut Folder" "${SHORTCUTS}"
	WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "UninstallString" "$INSTDIR\uninstall.exe"
	; This key sets the Version DWORD that new installers will check against
	WriteRegDWORD HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger" "Version" ${INSTALLERVERSION}

	CreateShortCut "$DESKTOP\ShimmerLogger.lnk" "$INSTDIR\ShimmerLogger.exe"
	CreateDirectory "$SMPROGRAMS\${SHORTCUTS}"
	CreateShortCut "$SMPROGRAMS\${SHORTCUTS}\ShimmerLogger.lnk" "$INSTDIR\ShimmerLogger.exe"
	CreateShortCut "$SMPROGRAMS\${SHORTCUTS}\uninstall.lnk" "$INSTDIR\uninstall.exe"
	;CreateShortCut "$SMPROGRAMS\${SHORTCUTS}\readme.lnk" "$INSTDIR\README_WINDOWS.txt"
SectionEnd

;-------------------------------------------
; Install the uninstaller (option is hidden)
Section -FinishSection
	WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

;-----------------------------------------------
; Uninstall section, deletes all installed files
Section "Uninstall"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ShimmerLogger"
	Delete "$INSTDIR\*.dll"
	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\ShimmerLogger.lnk"
	Delete "$SMPROGRAMS\${SHORTCUTS}\ShimmerLogger.lnk"
	Delete "$SMPROGRAMS\${SHORTCUTS}\uninstall.lnk"
	;Delete "$SMPROGRAMS\${SHORTCUTS}\readme.lnk"
	RMDir "$SMPROGRAMS\${SHORTCUTS}"
	
	; Clean up ShimmerLogger dir
	;Delete "$INSTDIR\README_WINDOWS.txt"
	Delete "$INSTDIR\ShimmerLogger.exe"
	;Delete "$INSTDIR\INSTALL.LOG"
	;Delete "$INSTDIR\COPYRIGHT.txt"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\${SHORTCUTS}"
	RMDir "$INSTDIR"

SectionEnd
