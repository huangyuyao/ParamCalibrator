TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = test

SOURCES += main.cpp

HEADERS += \
    ParamCalibrator.h

INCLUDEPATH += ../UserPanel
LIBS += ..\build-ParamCalibrator-64bit\release\ParamCalibrator.lib
