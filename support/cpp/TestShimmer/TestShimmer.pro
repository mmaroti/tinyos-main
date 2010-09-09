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

# INCLUDEPATH += ../qwtplot3d/include
QMAKE_LIBDIR += ../QextSerialPort/build

# DEFINES += QWT3D_DLL
CONFIG(release) += static
CONFIG(debug) += CONSOLE
CONFIG(debug, debug|release):LIBS += -lqextserialport
else:LIBS += -lqextserialport
win32:LIBS += -lsetupapi

# LIBS += ../qwtplot3d/lib/libqwtplot3d.a
DEFINES += QT_DLL

# QWT3D_DLL
QT += opengl
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
    src/GraphWidget.cpp \
    src/Graph.cpp \
    src/Solver.cpp \
    src/Data.cpp
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
    src/Solver.hpp \
    src/Data.hpp \
    src/ErrorCodes.hpp
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/CalibrationWidget.ui \
    src/ConsoleWidget.ui \
    src/DataWidget.ui \
    src/Widget3D.ui \
    src/GraphWidget.ui
RESOURCES += src/Resources.qrc \
    src/textures.qrc
