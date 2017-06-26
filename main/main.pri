# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to libHPCS.h, libHPCS shared library, Qwt project file
# and the boost library have to be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HVLPATH = "C:/Building/ECHMET/HVL_MT-bin"
BOOSTPATH = "C:/Building/boost_1_63_0"
QWTPATH = "C:/Building/Qwt-6.1.3-msvc-bin"
EIGENPATH = "C:/Building/eigen-3.3.3"
INCLUDEPATH += "$$HPCSPATH/include"
INCLUDEPATH += "$$HVLPATH/include"
INCLUDEPATH += $$EIGENPATH
DEPENDPATH += $$HPCSPATH

# Adjust the paths to Qwt library as needed.
include("$$QWTPATH/features/qwt.prf")

# Adjust the path to the libHPCS library as needed.
unix|win32: LIBS += -L"$$HVLPATH/lib" -llibhvl_mt
unix: LIBS += -lpthread

# Development version
DEFINES += UNSTABLE_VERSION
