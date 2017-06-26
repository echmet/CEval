#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T18:39:11
#
#-------------------------------------------------

QT       += widgets

TARGET = CSVSupport
TEMPLATE = lib

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

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    malformedcsvfiledialog.ui \
    malformedcsvfiledialog.ui \
    loadcsvfiledialog.ui

DESTDIR = ../../../efgbackends