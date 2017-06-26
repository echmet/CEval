#-------------------------------------------------
#
# Project created by QtCreator 2017-06-26T14:58:41
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = NetCDFSupport
TEMPLATE = lib

DEFINES += NETCDFSUPPORT_LIBRARY

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
        netcdfsupport.cpp \
    netcdffileloader.cpp

HEADERS += \
        netcdfsupport.h \
        netcdfsupport_global.h \ 
    netcdffileloader.h

DESTDIR = ../../../efgbackends

NETCDFPATH = /home/madcat/Devel/NetCDF-bin/
INCLUDEPATH += "$$NETCDFPATH/include"

LIBS += -L"$$NETCDFPATH/lib/" -lnetcdf
