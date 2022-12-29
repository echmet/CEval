WINARCH=""
win32_i386 {
    WINARCH=32
}
win32_x86_64 {
    WINARCH=64
}

# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to Qwt project file, HVL_MT library,
# the Eigen math library and the boost library shall be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
BOOSTPATH = "C:/Building/boost_1_75_0"
QWTPATH = "C:/Building/Qwt-6.2.0-msvc-bin-w$$WINARCH"
EIGENPATH = "C:/Building/eigen-3.4.0"
EDIIPATH = "C:/Building/ECHMET/EDII-bin-w32/include"
ECHMETUPDATERPATH = "C:/Building/ECHMET/ECHMETUpdateCheck-bin-w$$WINARCH"

INCLUDEPATH += $$BOOSTPATH
INCLUDEPATH += $$EIGENPATH
INCLUDEPATH += $$EDIIPATH
INCLUDEPATH += "$$ECHMETUPDATERPATH/include"
DEPENDPATH += "$$ECHMETUPDATERPATH/lib"

include("$$QWTPATH/features/qwt.prf")

unix|win32: LIBS += -L"$$ECHMETUPDATERPATH/lib" -llibECHMETUpdateCheck
unix: LIBS += -lpthread

# Development version
# DEFINES += UNSTABLE_VERSION

# Version released publically - if defined,
# some untested or experimental features will be
# hidden from the user
DEFINES += CEVAL_PUBLIC_RELEASE
