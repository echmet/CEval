#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T18:39:11
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = CSVSupport
TEMPLATE = lib
CONFIG += dll


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
