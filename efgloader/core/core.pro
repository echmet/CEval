QT += core dbus gui widgets network

CONFIG += c++11

TARGET = CEvalEFGLoader
CONFIG -= app_bundle

TEMPLATE = app

include(../efgloader.pri)

SOURCES += src/main.cpp \
    src/dataloader.cpp \
    src/ipcproxy.cpp \
    src/localsocketipcproxy.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    common/ipcinterface.h \
    common/backendinterface.h \
    src/dataloader.h \
    src/ipcproxy.h \
    src/localsocketipcproxy.h \

ipc_dbus {
    SOURCES += \
        src/dbusipcproxy.cpp \
        src/dbus/dbusinterface.cpp \
        src/dbus/DBusInterfaceAdaptor.cpp \
        src/dbus/DBusInterfaceInterface.cpp \

    HEADERS += \
        src/dbus/dbusinterface.h \
        src/dbusipcproxy.h \
        src/dbus/DBusInterfaceAdaptor.h \
        src/dbus/DBusInterfaceInterface.h \
}


DISTFILES += \
    src/dbus/dbusinterface.xml


DESTDIR = ../../
