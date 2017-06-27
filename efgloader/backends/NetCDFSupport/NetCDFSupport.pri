NETCDFPATH = "C:/Building/netCDF 4.4.1.1"
INCLUDEPATH += "$$NETCDFPATH/include"

LIBS += -L"$$NETCDFPATH/lib/" -lnetcdf
win32: LIBS += -luser32
