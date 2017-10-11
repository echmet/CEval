#-------------------------------------------------
#
# Project created by QtCreator 2017-06-17T19:29:53
#
#-------------------------------------------------

QT += core gui widgets

TARGET = HPCSSupport
TEMPLATE = lib
CONFIG += dll c++11

DEFINES += HPCSSUPPORT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        hpcssupport.cpp \
    loadchemstationdatadialog.cpp \
    chemstationbatchloader.cpp \
    chemstationbatchloadmodel.cpp \
    chemstationfileinfomodel.cpp \
    chemstationfileloader.cpp

HEADERS += \
        hpcssupport.h \
        hpcssupport_global.h \ 
    loadchemstationdatadialog.h \
    chemstationbatchloader.h \
    chemstationbatchloadmodel.h \
    chemstationfileinfomodel.h \
    chemstationfileloader.h

FORMS += \
    loadchemstationdatadialog.ui

DESTDIR = ../../../efgbackends

win32: LIBS += -luser32

!win32-msvc2015 {
    QMAKE_CXXFLAGS += "-std=c++11 -Wall -Wextra -pedantic"
} else {
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    QMAKE_CXXFLAGS += /openmp

    INCLUDEPATH += $$BOOSTPATH
}

include($$PWD/HPCSSupport.pri)
