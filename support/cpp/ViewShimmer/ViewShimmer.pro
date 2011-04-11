# -------------------------------------------------
# Project created by QtCreator 2011-03-21T10:34:18
# -------------------------------------------------
TARGET = ViewShimmer
TEMPLATE = app
INCLUDEPATH += ../QextSerialPort/
INCLUDEPATH += refreshtest/
INCLUDEPATH += c:\temp\qwt-6.0.0-rc5\src
QMAKE_LIBDIR += ../QextSerialPort/build
QMAKE_LIBDIR += c:\temp\qwt-6.0.0-rc5\lib
CONFIG(debug, debug|release):LIBS += -lqextserialport
else:LIBS += -lqextserialport
LIBS += -lqwtd
win32:LIBS += -lsetupapi
DEFINES += QT_DLL \
    QWT_DLL
SOURCES += main.cpp \
    Application.cpp \
    SerialListener.cpp \
    ConnectWidget.cpp \
    DataRecorder.cpp \
    signaldata.cpp \
    plot.cpp \
    knob.cpp \
    wheelbox.cpp \
    curvedata.cpp \
    mainwindow.cpp \
    scrollbar.cpp \
    scrollzoomer.cpp
HEADERS += Application.h \
    SerialListener.h \
    ConnectWidget.h \
    DataRecorder.h \
    constants.h \
    signaldata.h \
    plot.h \
    knob.h \
    wheelbox.h \
    curvedata.h \
    mainwindow.h \
    scrollbar.h \
    scrollzoomer.h
FORMS += mainwindow.ui \
    ConnectWidget.ui
