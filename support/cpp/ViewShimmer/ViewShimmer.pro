# -------------------------------------------------
# Project created by QtCreator 2011-03-21T10:34:18
# -------------------------------------------------
TARGET = ViewShimmer
TEMPLATE = app

INCLUDEPATH += ../qwt-6.0.0-rc5/src

QMAKE_LIBDIR += ../qwt-6.0.0-rc5/lib

LIBS += -lqwtd
win32:LIBS += -lsetupapi


DEFINES += QT_DLL \
    QWTD_DLL

SOURCES += main.cpp \
    Application.cpp \
    plot.cpp \
    curvedata.cpp \
    mainwindow.cpp \
    MoteData.cpp \
    MoteDataHolder.cpp \
    scrollzoomer.cpp \
    scrollbar.cpp \

HEADERS += Application.h \
    constants.h \
    plot.h \
    curvedata.h \
    mainwindow.h \
    MoteData.h \
    MoteDataHolder.h \
    scrollzoomer.h \
    scrollbar.h \

FORMS += mainwindow.ui
