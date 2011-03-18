; The name of the installer
Name "TestShimmer"
Icon "TestShimmer.ico"

; The file to write
OutFile "Install_TestShimmer.exe"

; The default installation directory
InstallDir $PROGRAMFILES\TestShimmer

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
  CreateDirectory $INSTDIR\calib

  SetOutPath $INSTDIR\bin
  
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
  File "sqlite3.exe"
  File "pthreadGCE2.dll"
  
  CreateShortCut "$DESKTOP\TestShimmer.lnk" "$INSTDIR\bin\TestShimmer.exe" "TestShimmer" "$INSTDIR\bin\TestShimmer.ico" 0
  
  CreateDirectory $INSTDIR\bin\sqldrivers
  SetOutPath $INSTDIR\bin\sqldrivers
  File "qsqlite4.dll"  
  
  ; Write the uninstall keys for Windows

  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\TestShimmer"
  CreateShortCut "$SMPROGRAMS\TestShimmer\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\TestShimmer\TestShimmer.lnk" "$INSTDIR\bin\TestShimmer.exe" "TestShimmer" "$INSTDIR\bin\TestShimmer.ico" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  

  ; Remove files and uninstaller
  Delete $INSTDIR\bin\sqldrivers\*.*
  Delete $INSTDIR\bin\*.*  
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\TestShimmer\*.*"
  
  ;Remove desktop shortcuts
  Delete "$DESKTOP\TestShimmer*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\TestShimmer"
  RMDir "$INSTDIR"

SectionEnd
