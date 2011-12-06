# -------------------------------------------------
# Project created by QtCreator 2011-12-02T16:39:20
# -------------------------------------------------
QT -= gui
TARGET = CalibShimmer
CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH += ../TntJama/
TEMPLATE = app
HEADERS += TurntableCalibrationModule.h \
    StationaryCalibrationModule.h \
    constants.h \
    LinearEquations.h \
    MoteData.h \
    MoteDataHolder.h \
    Application.h
SOURCES += main.cpp \
    TurntableCalibrationModule.cpp \
    StationaryCalibrationModule.cpp \
    LinearEquations.cpp \
    MoteData.cpp \
    MoteDataHolder.cpp \
    Application.cpp
