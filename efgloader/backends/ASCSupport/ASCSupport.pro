#-------------------------------------------------
#
# Project created by QtCreator 2017-10-25T21:03:18
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ASCSupport
TEMPLATE = lib
CONFIG += dll c++11

DEFINES += ASCSUPPORT_LIBRARY

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
    ascsupport.cpp

HEADERS +=\
    ascsupport.h \
    ascsupport_global.h \
    ascsupport_handlers.h

DESTDIR = ../../../efgbackends

win32: LIBS += -luser32

!win32-msvc2015 {
    QMAKE_CXXFLAGS += "-Wall -Wextra -pedantic"
} else {
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    QMAKE_CXXFLAGS += /openmp

    INCLUDEPATH += $$BOOSTPATH
}


