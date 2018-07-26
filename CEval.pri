# Configure paths to CEval dependencies here

# Adjust these include paths as needed.
# Paths to Qwt project file, HVL_MT library,
# the Eigen math library and the boost library shall be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
HVLPATH = "C:\Building\ECHMET\HVL_MT-bin"
BOOSTPATH = "C:\Building\boost_1_67_0"
QWTPATH = "C:\Building\Qwt-6.1.3-msvc-bin-w32"
EIGENPATH = "C:\Building\eigen-3.3.4"
EDIIPATH = "C:\Building\ECHMET\EDII-bin-w32\include"
ECHMETUPDATERPATH = "C:\Building\ECHMET\ECHMETUpdateCheck-bin-w32"

INCLUDEPATH += "$$HVLPATH/include"
INCLUDEPATH += $$EIGENPATH
INCLUDEPATH += $$EDIIPATH
INCLUDEPATH += "$$ECHMETUPDATERPATH/include"
DEPENDPATH += $$HVLPATH

include("$$QWTPATH/features/qwt.prf")

unix|win32: LIBS += -L"$$HVLPATH/lib" -llibhvl_mt -L"$$ECHMETUPDATERPATH/lib" -lECHMETUpdateCheck
unix: LIBS += -lpthread

# Development version
#DEFINES += UNSTABLE_VERSION

# Version released publically - if defined,
# some untested or experimental features will be
# hidden from the user
DEFINES += CEVAL_PUBLIC_RELEASE
