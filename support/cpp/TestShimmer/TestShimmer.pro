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

# QWT3D_DLL
# QT += opengl
# isEmpty( ISQT4 ):CONFIG += opengl
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
    src/GraphWidget.cpp \
    src/Graph.cpp \
    src/SDownloadWidget.cpp \
    src/Solver/Solver.cpp \
    src/Solver/Results.cpp \
    src/Solver/EulerAngles.cpp \
    src/Solver/DataWriter.cpp \
    src/Solver/DataReader.cpp \
    src/Solver/Data.cpp \
    src/SDCard/Win32BlockDevice.c \
    src/SDCard/Utility.cpp \
    src/SDCard/Tracker.cpp \
    src/SDCard/SDCardImpl.cpp \
    src/SDCard/SDCard.cpp \
    src/SDCard/Sample.cpp \
    src/SDCard/Header.cpp \
    src/SDCard/BlockIterator.cpp \
    src/SDCard/BlockDevice.cpp \
    src/SDCard/BlockChecker.cpp
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
    src/GraphWidget.h \
    src/Graph.h \
    src/constants.h \
    src/Widget3D.h \
    src/SDownloadWidget.h \
    src/Solver/Solver.hpp \
    src/Solver/Results.hpp \
    src/Solver/MatrixVector.hpp \
    src/Solver/EulerAngles.hpp \
    src/Solver/DataWriter.hpp \
    src/Solver/DataWriteException.hpp \
    src/Solver/DataReadException.hpp \
    src/Solver/DataReader.hpp \
    src/Solver/Data.hpp \
    src/Solver/CompileTimeConstants.hpp \
    src/SDCard/Utility.hpp \
    src/SDCard/Win32BlockDevice.h \
    src/SDCard/TypeDefs.hpp \
    src/SDCard/Tracker.hpp \
    src/SDCard/SDCardImpl.hpp \
    src/SDCard/SDCard.hpp \
    src/SDCard/Sample.hpp \
    src/SDCard/Header.hpp \
    src/SDCard/Constants.hpp \
    src/SDCard/BlockRelatedConsts.hpp \
    src/SDCard/BlockIterator.hpp \
    src/SDCard/BlockDevice.hpp \
    src/SDCard/BlockChecker.hpp
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/CalibrationWidget.ui \
    src/ConsoleWidget.ui \
    src/DataWidget.ui \
    src/Widget3D.ui \
    src/GraphWidget.ui \
    src/SDownloadWidget.ui
RESOURCES += src/Resources.qrc \
    src/textures.qrc
