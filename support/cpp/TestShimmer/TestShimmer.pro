# -------------------------------------------------
# Project created by QtCreator 2010-02-14T20:17:59
# -------------------------------------------------
TEMPLATE = app
TARGET = TestShimmer
INCLUDEPATH += ../QextSerialPort/
QMAKE_LIBDIR += ../QextSerialPort/build
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
    src/CalibrationWidget.cpp
HEADERS += src/DataRecorder.h \
    src/MainWindow.h \
    src/ConnectWidget.h \
    src/SerialListener.h \
    src/RawDataWidget.h \
    src/RawDataPlot.h \
    src/Application.h \
    src/PlotScrollArea.h \
    src/CalibrationWidget.h
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/RawDataWidget.ui \
    src/CalibrationWidget.ui
RESOURCES += src/Resources.qrc
