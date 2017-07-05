# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to Qwt project file, HVL_MT library,
# the Eigen math library and the boost library shall be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HVLPATH = "/home/madcat/Devel/ECHMET/HVL_MT-bin"
BOOSTPATH = "/home/madcat/Devel/ECHMET/CEval/includes"
QWTPATH = "/home/madcat/Devel/ECHMET/qwt-6.1.3-bin"
EIGENPATH = "/home/madcat/Devel/eigen-3.3.3"
INCLUDEPATH += "$$HPCSPATH/include"
INCLUDEPATH += "$$HVLPATH/include"
INCLUDEPATH += $$EIGENPATH
DEPENDPATH += $$HPCSPATH

include("$$QWTPATH/features/qwt.prf")

unix|win32: LIBS += -L"$$HVLPATH/lib" -lhvl_mt
unix: LIBS += -lpthread

# Development version
DEFINES += UNSTABLE_VERSION
