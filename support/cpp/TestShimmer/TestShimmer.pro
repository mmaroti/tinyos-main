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
QMAKE_LIBDIR += ../QextSerialPort/build
CONFIG(debug) += CONSOLE
CONFIG(debug, debug|release):LIBS += -lqextserialportd
else:LIBS += -lqextserialport
win32:LIBS += -lsetupapi
SOURCES += src/DataRecorder.cpp \
    src/MainWindow.cpp \
    src/main.cpp \
    src/ConnectWidget.cpp \
    src/SerialListener.cpp \
    src/RawDataWidget.cpp \
    src/RawDataPlot.cpp \
    src/Application.cpp \
    src/PlotScrollArea.cpp \
    src/CalibrationWidget.cpp \
    src/ConsoleWidget.cpp \
    src/LinearEquations.cpp \
    src/CalibrationModule.cpp \
    src/CalibratedDataWidget.cpp \
    src/CalibratedDataPlot.cpp
HEADERS += src/DataRecorder.h \
    src/MainWindow.h \
    src/ConnectWidget.h \
    src/SerialListener.h \
    src/RawDataWidget.h \
    src/RawDataPlot.h \
    src/Application.h \
    src/PlotScrollArea.h \
    src/CalibrationWidget.h \
    src/ConsoleWidget.h \
    src/LinearEquations.h \
    src/CalibrationModule.h \
    src/CalibratedDataWidget.h \
    src/CalibratedDataPlot.h
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/RawDataWidget.ui \
    src/CalibratedDataWidget.ui \
    src/CalibrationWidget.ui \
    src/ConsoleWidget.ui
RESOURCES += src/Resources.qrc
