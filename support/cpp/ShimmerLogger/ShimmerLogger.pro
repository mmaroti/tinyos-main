TEMPLATE = app
TARGET = ShimmerLogger 
DEPENDPATH += .
INCLUDEPATH += . \
    includes
CONFIG += qt thread release
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
win32:CONFIG += windows
win32:LIBS += -Llib_win \
    -lqwt \
    -lqextserialport1
unix:LIBS += -Llib \
    -lqwt \
    -lqextserialport

# Input
HEADERS += ClockWidget.h \
    configure.h \
    ShimmerLogger.h \
    PlotWidget.h \
	PortListener.h \
    DeviceManager.h
SOURCES += ClockWidget.cpp \
    ShimmerLogger.cpp \
    PlotWidget.cpp \
	PortListener.cpp \
    main.cpp \
    DeviceManager.cpp
