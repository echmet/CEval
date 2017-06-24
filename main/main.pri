# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to libHPCS.h, libHPCS shared library, Qwt project file
# and the boost library have to be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HPCSPATH = /home/madcat/Devel/ECHMET/libHPCS-bin
HVLPATH = /home/madcat/Devel/ECHMET/HVL_MT-bin
BOOSTPATH = $$PWD/includes
QWTPATH = "/home/madcat/Devel/ECHMET/qwt-6.1-bin"
EIGENPATH = "/home/madcat/Devel/eigen-3.3.3"
INCLUDEPATH += "$$HPCSPATH/include"
INCLUDEPATH += "$$HVLPATH/include"
INCLUDEPATH += $$EIGENPATH
DEPENDPATH += $$HPCSPATH

# Adjust the paths to Qwt library as needed.
include("$$QWTPATH/features/qwt.prf")

# Adjust the path to the libHPCS library as needed.
unix|win32: LIBS += -L"$$HPCSPATH/lib" -lHPCS -L"$$HVLPATH/lib" -lhvl_mt -lpthread

# Development version
DEFINES += UNSTABLE_VERSION

##  HACK
NETCDFPATH = /home/madcat/Devel/NetCDF-bin/
INCLUDEPATH += "$$NETCDFPATH/include"

LIBS += -L"$$NETCDFPATH/lib/" -lnetcdf
