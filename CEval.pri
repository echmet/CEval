# Configure paths to CEval dependencies here

# Adjust the paths to Qwt library as needed.
include(/home/madcat/Devel/ECHMET/qwt-6.1-bin/features/qwt.prf)

# Adjust the path to the libHPCS library as needed.
unix|win32: LIBS += -L$$PWD/libHPCS/ -lHPCS -lpthread

# Adjust these include paths as needed.
# Paths to libHPCS.h and the boost library
# have to be provided.
# BOOSTPATH variable is supposed to point to
# a directory that contains a "boost" subdirectory
# with the boost libraries.
INCLUDEPATH += $$PWD/libHPCS
DEPENDPATH += $$PWD/libHPCS
BOOSTPATH = $$PWD/includes

