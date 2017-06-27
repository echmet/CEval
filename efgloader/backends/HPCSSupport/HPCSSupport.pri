HPCSPATH = "C:/Building/ECHMET/libHCPS-bin"
INCLUDEPATH += "$$HPCSPATH/include"
unix|win32: LIBS += -L"$$HPCSPATH/lib" -lHPCS
win32: LIBS += -luser32
