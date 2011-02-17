; The name of the installer
Name "TestShimmer"
Icon "TestShimmer.ico"

; The file to write
OutFile "Install_TestShimmer.exe"

; The default installation directory
InstallDir $PROGRAMFILES\TestShimmer

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_TestShimmer" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "TestShimmer (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Create /rec /tmp /bin directories
  CreateDirectory $INSTDIR\rec
  CreateDirectory $INSTDIR\tmp
  CreateDirectory $INSTDIR\bin

  CreateDirectory $INSTDIR\app
  SetOutPath $INSTDIR\app
  
  ; Put file there
  File "TestShimmer.exe"
  File "gyro.exe"
  File "libgcc_s_dw2-1.dll"
  File "mingwm10.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtOpenGL4.dll"
  File "QtSql4.dll"
  File "TestShimmer.ico"
  
  CreateShortCut "$DESKTOP\TestShimmer.lnk" "$INSTDIR\app\TestShimmer.exe" "TestShimmer" "$INSTDIR\app\TestShimmer.ico" 0
  
  CreateDirectory $INSTDIR\app\sqldrivers
  SetOutPath $INSTDIR\app\sqldrivers
  File "qsqlite4.dll"  
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_TestShimmer "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestShimmer" "DisplayName" "NSIS TestShimmer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestShimmer" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestShimmer" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestShimmer" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\TestShimmer"
  CreateShortCut "$SMPROGRAMS\TestShimmer\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\TestShimmer\TestShimmer.lnk" "$INSTDIR\app\TestShimmer.exe" "TestShimmer" "$INSTDIR\app\TestShimmer.ico" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestShimmer"
  DeleteRegKey HKLM SOFTWARE\NSIS_TestShimmer

  ; Remove files and uninstaller
  Delete $INSTDIR\app\*.*
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\TestShimmer\*.*"
  
  ;Remove desktop shortcuts
  Delete "$DESKTOP\TestShimmer*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\TestShimmer"
  RMDir "$INSTDIR"

SectionEnd
