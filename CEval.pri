# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to libHPCS.h, libHPCS shared library, Qwt project file
# and the boost library have to be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HPCSPATH = $$PWD/libHPCS
HVLPATH = $$PWD/libhvl
BOOSTPATH = $$PWD/includes
QWTPATH = "/home/echmet/Devel/qwt-6.1-bin"
EIGENPATH = "/home/echmet/Devel/eigen-3.3.3"
INCLUDEPATH += $$HPCSPATH
INCLUDEPATH += $$HVLPATH
INCLUDEPATH += $$EIGENPATH
DEPENDPATH += $$HPCSPATH

# Adjust the paths to Qwt library as needed.
include("$$QWTPATH/features/qwt.prf")

# Adjust the path to the libHPCS library as needed.
unix|win32: LIBS += -L$$HPCSPATH -lHPCS -L$$HVLPATH -lhvl_mt -lpthread

# Development version
DEFINES += UNSTABLE_VERSION
