# -------------------------------------------------
# Project created by QtCreator 2011-03-21T10:34:18
# -------------------------------------------------
TARGET = ViewShimmer
TEMPLATE = app
INCLUDEPATH += ../TntJama/
INCLUDEPATH += ../QextSerialPort/
INCLUDEPATH += SDCard/
INCLUDEPATH += c:\Qwt-6.0.0\include
QMAKE_LIBDIR += ../QextSerialPort/build

# See http://doc.qt.nokia.com/4.7/qmake-advanced-usage.html#adding-new-configuration-features
# and qwt-6.0.0/INSTALL, last section "Using Qwt"
CONFIG += qwt
CONFIG(debug, debug|release):LIBS += -lqextserialport
else:LIBS += -lqextserialport
win32:LIBS += -lsetupapi
DEFINES += QT_DLL
SOURCES += main.cpp \
    Application.cpp \
    plot.cpp \
    curvedata.cpp \
    mainwindow.cpp \
    MoteData.cpp \
    MoteDataHolder.cpp \
    scrollzoomer.cpp \
    scrollbar.cpp \
    SDCard/Header.cpp \
    SDCard/FlatFileDB.cpp \
    SDCard/FileAsBlockDevice.cpp \
    SDCard/Console.cpp \
    SDCard/BlockIterator.cpp \
    SDCard/BlockChecker.cpp \
    SDCard/VirtualMoteID.cpp \
    SDCard/Utility.cpp \
    SDCard/Tracker.cpp \
    SDCard/TimeSyncReader.cpp \
    SDCard/TimeSyncMerger.cpp \
    SDCard/TimeSyncInfo.cpp \
    SDCard/SDCardImpl.cpp \
    SDCard/SDCard.cpp \
    SDCard/Sample.cpp \
    SDCard/RecordPairID.cpp \
    SDCard/RecordID.cpp \
    SDCard/Merger.cpp \
    SDCard/WinBlockDevice.c \
    SDCard/Win32BlockDevice.cpp \
    SDCard/Writer.cpp \
    SDCard/TimeSyncCalc.cpp \
    SDCard/DownloadTask.cpp \
    SDCard/DownloadManager.cpp \
    SDCard/SDCardCreator.cpp \
    SDCard/MoteRegistrar.cpp \
    SDCard/TimeSyncMsgReceiver.cpp \
    SDCard/RootDirPath.cpp \
    SDCard/MoteInfo.cpp \
    SDCard/TimeSyncDB.cpp \
    SDCard/RecordScout.cpp \
    SDCard/Line.cpp \
    SDCard/RecordInfo.cpp \
    SDCard/RecordList.cpp \
    SDCard/MoteHeader.cpp \
    SDCard/RecordLine.cpp \
    SDCard/RecordLinker.cpp \
    LinearEquations.cpp \
    SerialListener.cpp \
    ActiveMessage.cpp \
    SDataWidget.cpp \
    DriveSelectorDialog.cpp \
    ConnectWidget.cpp \
    DataRecorder.cpp
HEADERS += Application.h \
    constants.h \
    plot.h \
    curvedata.h \
    mainwindow.h \
    MoteData.h \
    MoteDataHolder.h \
    scrollzoomer.h \
    scrollbar.h \
    SDCard/FileAsBlockDevice.hpp \
    SDCard/BlockRelatedConsts.hpp \
    SDCard/Constants.hpp \
    SDCard/Console.hpp \
    SDCard/BlockIterator.hpp \
    SDCard/BlockDevice.hpp \
    SDCard/BlockChecker.hpp \
    SDCard/Utility.hpp \
    SDCard/TypeDefs.hpp \
    SDCard/Tracker.hpp \
    SDCard/TimeSyncReader.hpp \
    SDCard/TimeSyncMerger.hpp \
    SDCard/TimeSyncInfo.hpp \
    SDCard/TimeSyncConsts.hpp \
    SDCard/SDCardImpl.hpp \
    SDCard/SDCard.hpp \
    SDCard/Sample.hpp \
    SDCard/RecordPairID.hpp \
    SDCard/RecordID.hpp \
    SDCard/Merger.hpp \
    SDCard/Header.hpp \
    SDCard/FlatFileDB.hpp \
    SDCard/WinBlockDevice.h \
    SDCard/Win32BlockDevice.hpp \
    SDCard/VirtualMoteID.hpp \
    SDCard/TimeSyncCalc.hpp \
    SDCard/DownloadTask.hpp \
    SDCard/DownloadManager.hpp \
    SDCard/SDCardCreator.hpp \
    SDCard/MoteRegistrar.hpp \
    SDCard/TimeSyncMsgReceiver.hpp \
    SDCard/MoteRegistrar.hpp \
    SDCard/TimeSyncDB.hpp \
    SDCard/RecordScout.hpp \
    SDCard/MoteInfo.hpp \
    SDCard/Line.hpp \
    SDCard/RecordInfo.hpp \
    SDCard/RecordList.hpp \
    SDCard/MoteHeader.hpp \
    SDCard/RecordLine.hpp \
    SDCard/TimeSyncData.hpp \
    SDCard/RecordLinker.hpp \
    LinearEquations.h \
    SerialListener.h \
    ActiveMessage.hpp \
    SDataWidget.h \
    DriveSelectorDialog.h \
    ConnectWidget.h \
    DataRecorder.h
FORMS += mainwindow.ui \
    SDataWidget.ui \
    DriveSelectorDialog.ui \
    ConnectWidget.ui
