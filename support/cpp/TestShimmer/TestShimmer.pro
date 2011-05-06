# -------------------------------------------------
# Project created by QtCreator 2010-02-14T20:17:59
# -------------------------------------------------
TEMPLATE = app
TARGET = TestShimmer

# QMAKE_LIBDIR += c:\Octave\3.2.4_gcc-4.4.0\lib\octave-3.2.4\
# LIBS += -lliboctave -lliboctinterp
# INCLUDEPATH += c:\Octave\3.2.4_gcc-4.4.0\include\octave-3.2.4\octave\
# INCLUDEPATH += c:\Octave\3.2.4_gcc-4.4.0\include
INCLUDEPATH += ../TntJama/
INCLUDEPATH += ../QextSerialPort/
INCLUDEPATH += src/
INCLUDEPATH += src/Solver/
INCLUDEPATH += src/SDCard/
INCLUDEPATH += src/OpenGL/
INCLUDEPATH += src/SQL/
INCLUDEPATH += src/Ellipsoid/

# INCLUDEPATH += ../qwtplot3d/include
QMAKE_LIBDIR += ../QextSerialPort/build

# DEFINES += QWT3D_DLL
CONFIG(release) += static
CONFIG(release) += fexceptions
CONFIG(debug) += CONSOLE
CONFIG(debug, debug|release):LIBS += -lqextserialport
else:LIBS += -lqextserialport
win32:LIBS += -lsetupapi

# LIBS += ../qwtplot3d/lib/libqwtplot3d.a
DEFINES += QT_DLL
QT += opengl
QT += sql
isEmpty( ISQT4 ):CONFIG += opengl
SOURCES += src/DataRecorder.cpp \
    src/MainWindow.cpp \
    src/main.cpp \
    src/ConnectWidget.cpp \
    src/SerialListener.cpp \
    src/Application.cpp \
    src/PlotScrollArea.cpp \
    src/CalibrationWidget.cpp \
    src/ConsoleWidget.cpp \
    src/LinearEquations.cpp \
    src/StationaryCalibrationModule.cpp \
    src/PeriodicalCalibrationModule.cpp \
    src/DataWidget.cpp \
    src/DataPlot.cpp \
    src/TurntableCalibrationModule.cpp \
    src/GLWidget.cpp \
    src/window.cpp \
    src/Widget3D.cpp \
    src/Solver/Solver.cpp \
    src/Solver/Results.cpp \
    src/Solver/EulerAngles.cpp \
    src/Solver/DataWriter.cpp \
    src/Solver/DataReader.cpp \
    src/Solver/Data.cpp \
    src/SDataWidget.cpp \
    src/SDCard/Header.cpp \
    src/SDCard/FlatFileDB.cpp \
    src/SDCard/FileAsBlockDevice.cpp \
    src/SDCard/Console.cpp \
    src/SDCard/BlockIterator.cpp \
    src/SDCard/BlockChecker.cpp \
    src/SDCard/VirtualMoteID.cpp \
    src/SDCard/Utility.cpp \
    src/SDCard/Tracker.cpp \
    src/SDCard/TimeSyncReader.cpp \
    src/SDCard/TimeSyncMerger.cpp \
    src/SDCard/TimeSyncInfo.cpp \
    src/SDCard/SDCardImpl.cpp \
    src/SDCard/SDCard.cpp \
    src/SDCard/Sample.cpp \
    src/SDCard/RecordPairID.cpp \
    src/SDCard/RecordID.cpp \
    src/SDCard/Merger.cpp \
    src/SDCard/WinBlockDevice.c \
    src/SDCard/Win32BlockDevice.cpp \
    src/SDCard/Writer.cpp \
    src/SDCard/TimeSyncCalc.cpp \
    src/SDCard/DownloadTask.cpp \
    src/SDCard/DownloadManager.cpp \
    src/SDCard/SDCardCreator.cpp \
    src/SDCard/MoteRegistrar.cpp \
    src/SDCard/TimeSyncMsgReceiver.cpp \
    src/SDCard/RootDirPath.cpp \
    src/LogWidget.cpp \
    src/LogWidgetBasic.cpp \
    src/SDCard/MoteInfo.cpp \
    src/SDCard/TimeSyncDB.cpp \
    src/SDCard/RecordScout.cpp \
    src/SDCard/Line.cpp \
    src/SDCard/RecordInfo.cpp \
    src/SDCard/RecordList.cpp \
    src/SDCard/MoteHeader.cpp \
    src/SDCard/RecordLine.cpp \
    src/TabWatcher.cpp \
    src/Solver/ConnectionState.cpp \
    src/Solver/MockSolver.cpp \
    src/OpenGL/DataHolder.cpp \
    src/OpenGL/GLWindow.cpp \
    src/DriveSelectorDialog.cpp \
    src/SQL/SQLDialog.cpp \
    src/SQL/RecordHandler.cpp \
    src/OpenGL/GLElbowFlexWidget.cpp \
    src/SDCard/RecordLinker.cpp \
    src/Ellipsoid/AccelMagMsgReceiver.cpp \
    src/ActiveMessage.cpp \
    src/Ellipsoid/EllipsoidCalibration.cpp \
    src/Ellipsoid/AccelMagSample.cpp \
    src/OpenGL/ArmWidget.cpp \
    src/Solver/MatrixVector.cpp \
    src/Solver/MatrixVectorInstantiation.cpp
HEADERS += src/DataRecorder.h \
    src/MainWindow.h \
    src/ConnectWidget.h \
    src/SerialListener.h \
    src/Application.h \
    src/PlotScrollArea.h \
    src/CalibrationWidget.h \
    src/ConsoleWidget.h \
    src/LinearEquations.h \
    src/StationaryCalibrationModule.h \
    src/PeriodicalCalibrationModule.h \
    src/DataWidget.h \
    src/DataPlot.h \
    src/TurntableCalibrationModule.h \
    src/GLWidget.h \
    src/window.h \
    src/constants.h \
    src/Widget3D.h \
    src/Solver/Solver.hpp \
    src/Solver/Results.hpp \
    src/Solver/MatrixVector.hpp \
    src/Solver/EulerAngles.hpp \
    src/Solver/DataReader.hpp \
    src/Solver/Data.hpp \
    src/Solver/CompileTimeConstants.hpp \
    src/SDataWidget.h \
    src/SDCard/FileAsBlockDevice.hpp \
    src/SDCard/BlockRelatedConsts.hpp \
    src/SDCard/Constants.hpp \
    src/SDCard/Console.hpp \
    src/SDCard/BlockIterator.hpp \
    src/SDCard/BlockDevice.hpp \
    src/SDCard/BlockChecker.hpp \
    src/SDCard/Utility.hpp \
    src/SDCard/TypeDefs.hpp \
    src/SDCard/Tracker.hpp \
    src/SDCard/TimeSyncReader.hpp \
    src/SDCard/TimeSyncMerger.hpp \
    src/SDCard/TimeSyncInfo.hpp \
    src/SDCard/TimeSyncConsts.hpp \
    src/SDCard/SDCardImpl.hpp \
    src/SDCard/SDCard.hpp \
    src/SDCard/Sample.hpp \
    src/SDCard/RecordPairID.hpp \
    src/SDCard/RecordID.hpp \
    src/SDCard/Merger.hpp \
    src/SDCard/Header.hpp \
    src/SDCard/FlatFileDB.hpp \
    src/SDCard/WinBlockDevice.h \
    src/SDCard/Win32BlockDevice.hpp \
    src/SDCard/VirtualMoteID.hpp \
    src/SDCard/TimeSyncCalc.hpp \
    src/SDCard/DownloadTask.hpp \
    src/SDCard/DownloadManager.hpp \
    src/SDCard/SDCardCreator.hpp \
    src/SDCard/MoteRegistrar.hpp \
    src/SDCard/TimeSyncMsgReceiver.hpp \
    src/SDCard/MoteRegistrar.hpp \
    src/LogWidget.h \
    src/LogWidgetBasic.h \
    src/SDCard/TimeSyncDB.hpp \
    src/SDCard/RecordScout.hpp \
    src/SDCard/MoteInfo.hpp \
    src/SDCard/Line.hpp \
    src/SDCard/RecordInfo.hpp \
    src/SDCard/RecordList.hpp \
    src/SDCard/MoteHeader.hpp \
    src/SDCard/RecordLine.hpp \
    src/TabWatcher.hpp \
    src/Solver/ConnectionState.hpp \
    src/Solver/StateColor.hpp \
    src/Solver/Range.hpp \
    src/Solver/MockSolver.hpp \
    src/OpenGL/GLWindow.hpp \
    src/OpenGL/DataHolder.hpp \
    src/DriveSelectorDialog.h \
    src/SQL/SQLDialog.hpp \
    src/SQL/RecordHandler.hpp \
    src/SQL/Person.hpp \
    src/SQL/MotionTypes.hpp \
    src/SQL/CustomSqlQueryModel.hpp \
    src/OpenGL/ElbowFlexSign.hpp \
    src/OpenGL/AnimationElbowFlexSign.hpp \
    src/OpenGL/GLElbowFlexWidget.hpp \
    src/Range.hpp \
    src/SDCard/TimeSyncData.hpp \
    src/SDCard/RecordLinker.hpp \
    src/Ellipsoid/AccelMagMsgReceiver.hpp \
    src/ActiveMessage.hpp \
    src/Ellipsoid/EllipsoidCalibration.hpp \
    src/Ellipsoid/AccelMagSample.hpp \
    src/OpenGL/ArmWidget.hpp
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/CalibrationWidget.ui \
    src/ConsoleWidget.ui \
    src/DataWidget.ui \
    src/Widget3D.ui \
    src/SDataWidget.ui \
    src/LogWidget.ui \
    src/LogWidgetBasic.ui \
    src/LogDialog.ui \
    src/DriveSelectorDialog.ui \
    src/EllipsoidCalibration.ui
RESOURCES += src/Resources.qrc \
    src/textures.qrc
