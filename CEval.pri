# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to Qwt project file, HVL_MT library,
# the Eigen math library and the boost library shall be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HVLPATH = "/home/madcat/Devel/ECHMET/HVL_MT-bin"
BOOSTPATH = "/home/madcat/Devel/ECHMET/boost_1_66_0"
QWTPATH = "/home/madcat/Devel/ECHMET/qwt-6.1.3-bin"
EIGENPATH = "/home/madcat/Devel/eigen-3.3.4"
EDIIPATH = "/home/madcat/Devel/ECHMET/EDII-bin/include"

INCLUDEPATH += "$$HVLPATH/include"
INCLUDEPATH += $$EIGENPATH
INCLUDEPATH += $$EDIIPATH
DEPENDPATH += $$HPCSPATH

include("$$QWTPATH/features/qwt.prf")

unix|win32: LIBS += -L"$$HVLPATH/lib" -lhvl_mt
unix: LIBS += -lpthread

# Development version
DEFINES += UNSTABLE_VERSION

# Version released publically - if defined,
# some untested or experimental features will be
# hidden from the user
DEFINES += CEVAL_PUBLIC_RELEASE
