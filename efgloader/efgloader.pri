linux {
    #DEFINES += ENABLE_IPC_INTERFACE_DBUS
    #CONFIG += ipc_dbus
}

win32-msvc2015 {
    DEFINES += CEVAL_COMPILER_MSVC
} else {
    DEFINES += CEVAL_COMPILER_GCC_LIKE
}

