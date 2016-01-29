#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T12:30:13
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = CEval
TEMPLATE = app

DEFINES += _USE_MATH_DEFINES
unix {
    DEFINES += ECHMET_MATH_HVL_PLATFORM_UNIX
} win32 {
    DEFINES += ECHMET_MATH_HVL_PLATFORM_WIN
    DEFINES += ECHMET_MATH_HVL_MINGW32
}

CONFIG += xopenmp
xopenmp {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

CONFIG += xsimd
xsimd {
    QMAKE_CXXFLAGS += "-mmmx -msse -msse2"
}

SOURCES += src/main.cpp \
        src/gui/evalmainwindow.cpp \
    src/gui/common/floatingvaluelineedit.cpp \
    src/commonparametersengine.cpp \
    src/dataaccumulator.cpp \
    src/floatingvaluedelegate.cpp \
    src/evaluationengine.cpp \
    src/gui/common/qmappedcheckbox.cpp \
    src/gui/maincontrolswidget.cpp \
    src/gui/evaluationwidget.cpp \
    src/gui/commonparameterswidget.cpp \
    src/gui/loadchemstationdatadialog.cpp \
    src/chemstationfileinfomodel.cpp \
    src/chemstationfileloader.cpp \
    src/evaluationwidgetconnector.cpp \
    src/datafileloader.cpp \
    src/evalmainwindowconnector.cpp \
    src/gui/evaluatedpeakswidget.cpp \
    src/gui/selectpeakdialog.cpp \
    src/foundpeaksmodel.cpp \
    src/math/extremesearching.cpp \
    src/peakfinder.cpp \
    src/peakevaluator.cpp \
    src/math/functions.cpp \
    src/math/alglib/ap.cpp \
    src/math/alglib/chisquaredistr.cpp \
    src/math/alglib/gammaf.cpp \
    src/math/alglib/ibetaf.cpp \
    src/math/alglib/igammaf.cpp \
    src/math/alglib/normaldistr.cpp \
    src/math/alglib/studenttdistr.cpp \
    src/modecontext.cpp \
    src/modecontextlimited.cpp \
    src/evaluatedpeaksmodel.cpp \
    src/evaluatedpeakswidgetconnector.cpp \
    src/csvfileloader.cpp \
    src/gui/loadcsvfiledialog.cpp \
    src/commonparameterswidgetconnector.cpp \
    src/helpers.cpp \
    src/hvlcalculator.cpp \
    src/math/hvl.cpp \
    src/gui/hyperbolefitwidget.cpp \
    src/hyperbolefittingengine.cpp \
    src/hyperbolefittingwidgetconnector.cpp \
    src/maincontrolswidgetconnector.cpp \
    src/gui/addpeakdialog.cpp \
    src/evalserializable.cpp \
    src/globals.cpp \
    src/doubletostringconvertor.cpp \
    src/gui/numberformatdialog.cpp \
    src/gui/hvlfitinprogressdialog.cpp \
    src/gui/common/witchcraft.cpp \
    src/crashhandler.cpp \
    src/gui/crashhandlerdialog.cpp \
    src/gui/adjustplotvisualsdialog.cpp \
    src/custommetatypes.cpp \
    src/standardmodecontextsettingshandler.cpp \
    src/serieproperties.cpp \
    src/gui/aboutdialog.cpp \
    src/scrollareaeventfilter.cpp

HEADERS  += src/gui/evalmainwindow.h \
    src/gui/common/floatingvaluelineedit.h \
    src/commonparametersengine.h \
    src/dataaccumulator.h \
    src/commonparametersitems.h \
    src/floatingvaluedelegate.h \
    src/evaluationengine.h \
    src/abstractmappermodel.h \
    src/floatingmappermodel.h \
    src/booleanmappermodel.h \
    src/evaluationparametersitems.h \
    src/gui/common/qmappedcheckbox.h \
    src/evaluationenginemsgs.h \
    src/gui/maincontrolswidget.h \
    src/gui/evaluationwidget.h \
    src/gui/commonparameterswidget.h \
    src/gui/loadchemstationdatadialog.h \
    src/chemstationfileinfomodel.h \
    src/chemstationfileloader.h \
    src/comboboxmodel.h \
    src/evaluationwidgetconnector.h \
    src/datafileloader.h \
    src/evalmainwindowconnector.h \
    src/datafileloadermsgs.h \
    src/dynamiccomboboxmodel.h \
    src/gui/evaluatedpeakswidget.h \
    src/helpers.h \
    src/gui/selectpeakdialog.h \
    src/foundpeaksmodel.h \
    src/math/extremesearching.h \
    src/peakfinder.h \
    src/evaluationresultsitems.h \
    src/peakevaluator.h \
    src/math/hvlestimate.h \
    src/math/functions.h \
    src/math/bool_counter.h \
    src/math/alglib/alglib.h \
    src/math/alglib/ap.h \
    src/math/alglib/chisquaredistr.h \
    src/math/alglib/gammaf.h \
    src/math/alglib/ibetaf.h \
    src/math/alglib/igammaf.h \
    src/math/alglib/normaldistr.h \
    src/math/alglib/studenttdistr.h \
    src/modecontext.h \
    src/modecontextlimited.h \
    src/evaluatedpeaksmodel.h \
    src/evaluatedpeakswidgetconnector.h \
    src/math/regressor/regress.h \
    src/math/matrix/core.h \
    src/math/matrix/core.hpp \
    src/math/matrix/gauss/gauss.h \
    src/math/matrix/gauss/gauss.hpp \
    src/math/hvl.hpp \
    src/math/matrix/data.h \
    src/math/matrix/data.hpp \
    src/math/matrix/impl/cells_pointer.h \
    src/math/matrix/impl/cells_pointer.hpp \
    src/math/matrix/impl/core_row.h \
    src/math/matrix/impl/core_row.hpp \
    src/math/matrix/impl/core_rowspointer.h \
    src/math/matrix/impl/core_rowspointer.hpp \
    src/math/matrix/impl/data_row.h \
    src/math/matrix/impl/data_row.hpp \
    src/math/matrix/impl/data_rowspointer.h \
    src/math/matrix/impl/data_rowspointer.hpp \
    src/math/matrix/impl/rows_proxy.h \
    src/math/matrix/impl/rows_proxy.hpp \
    src/math/matrix/gauss/fwdcl.h \
    src/math/matrix/gauss/gauss_switch.h \
    src/math/matrix/gauss/gauss_switch.hpp \
    src/math/matrix/gauss/gauss_wrap.h \
    src/math/matrix/gauss/gauss_wrap.hpp \
    src/math/matrix/fwdcl.h \
    src/math/matrix/basic_dcl.h \
    src/dataaccumulatormsgs.h \
    src/math/mystd/rac_facade.h \
    src/math/mystd/rac_facade.hpp \
    src/math/mystd/bool_counter.h \
    src/math/mystd/bool_counter.hpp \
    src/math/mystd/class_version.hpp \
    src/math/mystd/ext.h \
    src/math/mystd/ext.hpp \
    src/math/mystd/indebug.h \
    src/math/mystd/indebugoff.h \
    src/math/mystd/indebugon.h \
    src/math/mystd/iterator_facade_ext.h \
    src/math/mystd/iterator_facade_ext.hpp \
    src/math/mystd/math_basics.h \
    src/math/mystd/math_basics.hpp \
    src/math/mystd/stddefs.h \
    src/math/mystd/tests.h \
    src/math/mystd/mem/base.h \
    src/math/mystd/mem/base.hpp \
    src/math/mystd/mem/mem.h \
    src/math/mystd/mem/mem.hpp \
    src/math/mystd/mem/shared_buffer.h \
    src/math/mystd/mem/shared_buffer.hpp \
    src/math/mystd/mem/release/advance.h \
    src/math/mystd/mem/release/advance.hpp \
    src/math/mystd/mem/release/guard.h \
    src/math/mystd/mem/release/guard.hpp \
    src/math/mystd/mem/release/noguard.h \
    src/math/mystd/mem/release/noguard.hpp \
    src/math/mystd/mem/create/guard.h \
    src/math/mystd/mem/create/guard.hpp \
    src/math/mystd/mem/create/noguard.h \
    src/math/mystd/mem/create/noguard.hpp \
    src/math/mystd/mem/copy/guard.h \
    src/math/mystd/mem/copy/guard.hpp \
    src/math/mystd/mem/copy/noguard.h \
    src/math/mystd/mem/copy/noguard.hpp \
    src/math/hvl_types.hpp \
    src/csvfileloader.h \
    src/gui/loadcsvfiledialog.h \
    src/commonparameterswidgetconnector.h \
    src/hvlcalculator.h \
    src/gui/hyperbolefitwidget.h \
    src/hyperbolefittingengine.h \
    src/stringmappermodel.h \
    src/hyperbolefittingwidgetconnector.h \
    src/integermappermodel.h \
    src/modecontexttypes.h \
    src/maincontrolswidgetconnector.h \
    src/hyperbolefititems.h \
    src/math/regressor/hyperbole.h \
    src/math/regressor/hyperbole2.h \
    src/gui/addpeakdialog.h \
    src/evalserializable.h \
    src/globals.h \
    src/math/regressor/profiler.h \
    src/doubletostringconvertor.h \
    src/gui/numberformatdialog.h \
    src/setup.h \
    src/math/regressor/hvlPeak.h \
    src/gui/hvlfitinprogressdialog.h \
    src/gui/common/witchcraft.h \
    src/mappedvectorwrapper.h \
    src/crashhandler.h \
    src/stacktrace.h \
    src/stacktrace_win.h \
    src/gui/crashhandlerdialog.h \
    src/gui/adjustplotvisualsdialog.h \
    src/custommetatypes.h \
    src/standardmodecontextsettingshandler.h \
    src/serieproperties.h \
    src/math/mystd/foostream.h \
    src/gui/aboutdialog.h \
    src/hyperbolefittingenginemsgs.h \
    src/scrollareaeventfilter.h \
    src/inumberformatchangeable.h

FORMS    += src/gui/evalmainwindow.ui \
    src/gui/maincontrolswidget.ui \
    src/gui/evaluationwidget.ui \
    src/gui/commonparameterswidget.ui \
    src/gui/loadchemstationdatadialog.ui \
    src/gui/evaluatedpeakswidget.ui \
    src/gui/selectpeakdialog.ui \
    src/gui/loadcsvfiledialog.ui \
    src/gui/hyperbolefitwidget.ui \
    src/gui/addpeakdialog.ui \
    src/gui/numberformatdialog.ui \
    src/gui/hvlfitinprogressdialog.ui \
    src/gui/crashhandlerdialog.ui \
    src/gui/adjustplotvisualsdialog.ui \
    src/gui/aboutdialog.ui

unix {
    LIBS += -ldl
} win32 {
    LIBS += -ldbghelp
}

include($$PWD/CEval.pri)

QMAKE_CXXFLAGS += "-std=c++11 -Wall -Wextra -pedantic -isystem $$BOOSTPATH"

