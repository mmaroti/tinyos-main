# -------------------------------------------------
# Project created by QtCreator 2011-03-21T10:34:18
# -------------------------------------------------
TARGET = ViewShimmer
TEMPLATE = app

# See http://doc.qt.nokia.com/4.7/qmake-advanced-usage.html#adding-new-configuration-features
# and qwt-6.0.0/INSTALL, last section "Using Qwt"
CONFIG += qwt

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
