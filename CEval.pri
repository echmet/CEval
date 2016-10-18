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
QWTPATH = "/home/madcat/Devel/ECHMET/qwt-6.1-bin/features/qwt.prf"
ARMAPATH = "/home/madcat/Devel/ECHMET/armadillo-bin"
INCLUDEPATH += $$HPCSPATH
INCLUDEPATH += $$HVLPATH
INCLUDEPATH += $$ARMAPATH/include
DEPENDPATH += $$HPCSPATH

# Adjust the paths to Qwt library as needed.
include($$QWTPATH)

# Adjust the path to the libHPCS library as needed.
unix|win32: LIBS += -L$$HPCSPATH -lHPCS -L$$HVLPATH -lhvl_mt /home/madcat/Devel/ECHMET/ref-LAPACK-bin/lib64/liblapack.a /home/madcat/Devel/ECHMET/ref-LAPACK-bin/lib64/libblas.a -lgfortran -lpthread

# Development version
DEFINES += UNSTABLE_VERSION
