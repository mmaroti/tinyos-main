# -------------------------------------------------
# Project created by QtCreator 2010-02-14T20:17:59
# -------------------------------------------------
TEMPLATE = app
TARGET = TestShimmer
INCLUDEPATH += lib/qextserialport
QMAKE_LIBDIR += lib/qextserialport
CONFIG(debug, debug|release):LIBS += -lqextserialportd
else:LIBS += -lqextserialport
unix:LIBS += -lqextserialport_unix
win32:LIBS += -lsetupapi
SOURCES += src/DataRecorder.cpp \
    src/MainWindow.cpp \
    src/main.cpp \
    src/ConnectWidget.cpp \
    src/SerialListener.cpp \
    src/RawDataWidget.cpp \
    src/RawDataPlot.cpp \
    src/Application.cpp \
    src/PlotScrollArea.cpp
HEADERS += src/DataRecorder.h \
    src/MainWindow.h \
    lib/qextserialport/qextserialenumerator.h \
    lib/qextserialport/qextserialport.h \
    src/ConnectWidget.h \
    src/SerialListener.h \
    src/RawDataWidget.h \
    src/RawDataPlot.h \
    src/Application.h \
    src/PlotScrollArea.h
FORMS += src/MainWindow.ui \
    src/ConnectWidget.ui \
    src/RawDataWidget.ui
RESOURCES += src/Resources.qrc
