#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T18:39:11
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = CSVSupport
TEMPLATE = lib
CONFIG += dll c++11


DEFINES += CSVSUPPORT_LIBRARY

SOURCES += csvsupport.cpp \
    csvfileloader.cpp \
    malformedcsvfiledialog.cpp \
    loadcsvfiledialog.cpp

HEADERS += csvsupport.h\
        csvsupport_global.h \
    csvfileloader.h \
    malformedcsvfiledialog.h \
    loadcsvfiledialog.h

FORMS += \
    malformedcsvfiledialog.ui \
    loadcsvfiledialog.ui

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
