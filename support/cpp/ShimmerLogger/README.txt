Copyright (C) 2010, University of Szeged
    * All rights reserved.
Author: Andras Becsi (abecsi@inf.u-szeged.hu) 

ShimmerLogger

ShimmerLogger is using Qwt (Qt Widgets for Technical Applications) and 
qextserialport.

Build Instructions:

WINDOWS:
Install Qt SDK 4.6.x or newer (http://qt.nokia.com/downloads).
This installs a mingw32 build environment which is able to build the source.

Download qextserialport from http://code.google.com/p/qextserialport/ 
and build it in dll release mode (you may need to edit qextserialport.pro)
using QtCreator or the cmd.exe shell.

Download Qwt from http://qwt.sourceforge.net and build it in staticlib release mode
(You also may need to edit the pro file here)

Copy the needed libs into lib_win and the needed dlls into lib_win\dll
(Needed by the nsis installer script)

Copy all header files of Qwt and qextserialport to includes directory!

Run qmake and build ShimmerLogger in QtCreator or with mingw32-make in cmd.exe.

To run it you need to copy some dll files next to the exe or set the library path to contain:
libgcc_s_dw2-1.dll (can be found in the mingw32 installation of the Qt SDK)
mingwm10.dll (ditto)
qextserialport1.dll (the dll of qextserialport)
QtCore4.dll (found in the Qt SDK install)
QtGui4.dll (ditto)

You need NSIS if you want to create a self extracting exe from intaller\intaller.nsi.
(http://nsis.sourceforge.net)

LINUX:
Download Qt SDK and install it, or use the shipped version of your distribution.
(4.6 or newer is supported, but the build may also work with older 4.x versions)
Download the dependencies (qextserial, Qwt), build them in staticlib release mode,
then copy the libs into lib directory.
Copy all header files of Qwt and qextserialport to includes directory!
Run qmake on ShimmerLogger.pro and build it using make in bash or QtCreator.

