HPCSPATH = "/home/echmet/Devel/libHPCS-bin/"
INCLUDEPATH += "$$HPCSPATH/include"
unix|win32: LIBS += -L"$$HPCSPATH/lib" -lHPCS
