#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T12:30:13
#
#-------------------------------------------------

QT       += core gui network printsupport widgets

CONFIG += c++11

TARGET = CEval
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _USE_MATH_DEFINES
unix {
    DEFINES += ECHMET_COMPILER_GCC_LIKE
} win32 {
    DEFINES += CRASHHANDLING_WIN32

    win32-msvc2015 {
        DEFINES += LIBHVL_COMPILER_MSVC
        DEFINES += ECHMET_COMPILER_MSVC
    } else {
        DEFINES += LIBHVL_COMPILER_MINGW
        DEFINES += ECHMET_COMPILER_GCC_LIKE
    }
}

linux {
    DEFINES += CRASHHANDLING_LINUX
    CONFIG += ipc_dbus
}

!win32-msvc2015 {
    CONFIG += xopenmp xsimd
}

xopenmp {
    linux-clang {
        QMAKE_CXXFLAGS += -fopenmp=libomp
        QMAKE_LFLAGS += -fopenmp=libomp
    } else {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -fopenmp
    }
}

xsimd {
    QMAKE_CXXFLAGS += -mmmx
    QMAKE_CXXFLAGS += -msse
    QMAKE_CXXFLAGS += -msse2
}

xsanitize {
    CONFIG += sanitizer
    CONFIG += sanitize_address
    CONFIG += sanitize_undefined
    CONFIG += sanitize_leak
}

contains(QT_ARCH, i386) {
    CONFIG += win32_i386
} else {
    CONFIG += win32_x86_64
}

CONFIG(release, debug|release) {
    DEFINES += USE_CRASHHANDLER

    linux {
        QMAKE_LFLAGS_RELEASE += "-rdynamic"
    }
}
CONFIG(debug, debug|release) {
    # Configuration specific for "debug" build

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
    src/evaluationwidgetconnector.cpp \
    src/evalmainwindowconnector.cpp \
    src/gui/evaluatedpeakswidget.cpp \
    src/gui/selectpeakdialog.cpp \
    src/foundpeaksmodel.cpp \
    src/math/extremesearching.cpp \
    src/peakevaluator.cpp \
    src/math/functions.cpp \
    src/math/alglib/ap.cpp \
    src/math/alglib/chisquaredistr.cpp \
    src/math/alglib/gammaf.cpp \
    src/math/alglib/ibetaf.cpp \
    src/math/alglib/igammaf.cpp \
    src/math/alglib/normaldistr.cpp \
    src/math/alglib/studenttdistr.cpp \
    src/plotcontext.cpp \
    src/plotcontextlimited.cpp \
    src/evaluatedpeaksmodel.cpp \
    src/evaluatedpeakswidgetconnector.cpp \
    src/commonparameterswidgetconnector.cpp \
    src/helpers.cpp \
    src/hvlcalculator.cpp \
    src/maincontrolswidgetconnector.cpp \
    src/gui/addpeakdialog.cpp \
    src/evalserializable.cpp \
    src/globals.cpp \
    src/doubletostringconvertor.cpp \
    src/gui/numberformatdialog.cpp \
    src/gui/hvlfitinprogressdialog.cpp \
    src/witchcraft.cpp \
    src/gui/crashhandlerdialog.cpp \
    src/gui/adjustplotvisualsdialog.cpp \
    src/custommetatypes.cpp \
    src/standardplotcontextsettingshandler.cpp \
    src/serieproperties.cpp \
    src/gui/aboutdialog.cpp \
    src/scrollareaeventfilter.cpp \
    src/gui/evaluationupperwidget.cpp \
    src/evaluationupperwidgetconnector.cpp \
    src/gui/setaxistitlesdialog.cpp \
    src/gui/exportdatatabletocsvdialog.cpp \
    src/plotexporter.cpp \
    src/gui/exportplottoimagedialog.cpp \
    src/abstractpeakfinder.cpp \
    src/assistedpeakfinder.cpp \
    src/manualpeakfinder.cpp \
    src/ploteventfilter.cpp \
    src/peakcontext.cpp \
    src/gui/common/collapseexpandbutton.cpp \
    src/gui/common/collapsiblegroupbox.cpp \
    src/gui/softwareupdatewidget.cpp \
    src/gui/autoupdatecheckdialog.cpp \
    src/gui/checkforupdatedialog.cpp \
    src/softwareupdater.cpp \
    src/evalmainwindowsoftwareupdaterconnector.cpp \
    src/doubleclickableqwtplotzoomer.cpp \
    src/evaluationenginedataexporter.cpp \
    src/dataexporter/exporterelems.cpp \
    src/dataexporter/exporter.cpp \
    src/dataexporter/schemeserializer.cpp \
    src/dataexporter/backends/abstractexporterbackend.cpp \
    src/dataexporter/backends/textexporterbackend.cpp \
    src/dataexporter/backends/htmlexporterbackend.cpp \
    src/dataexporter/schemesmanagerwidget.cpp \
    src/dataexporter/schemeeditor.cpp \
    src/gui/textexporterbackendconfigurationdialog.cpp \
    src/dataexporter/backends/textstreamexporterbackend.cpp \
    src/gui/appendoverwriteexportfilemessagebox.cpp \
    src/hvllibwrapper.cpp \
    src/gui/specifypeakboundariesdialog.cpp \
    src/hyperbolafittingengine.cpp \
    src/gui/hyperbolafitwidget.cpp \
    src/gui/registerinhyperbolafitdialog.cpp \
    src/gui/registerinhyperbolafitwidget.cpp \
    src/hyperbolafittingwidgetconnector.cpp \
    src/customlocalesortproxymodel.cpp \
    src/crashhandling/crashhandlerwindows.cpp \
    src/crashhandling/crashhandlerbase.cpp \
    src/crashhandling/nullcrashhandler.cpp \
    src/crashevent.cpp \
    src/cevalcrashhandler.cpp \
    src/crasheventcatcher.cpp \
    src/crashhandling/crashhandlerlinux.cpp \
    src/crashhandling/crashhandlerlinux_stacktrace.cpp \
    src/gui/hvlestimateinprogressdialog.cpp \
    src/math/hvlestimate.cpp \
    src/efg/efgloaderwatcher.cpp \
    src/efg/efgloaderinterface.cpp \
    src/efg/efgtypes.cpp \
    src/efg/ipcclient.cpp \
    src/efg/localsocketclient.cpp \
    src/hvlextrapolator.cpp \
    src/snrcalculator.cpp \
    src/datahash.cpp \
    src/gui/ediinotfounddialog.cpp \
    src/gui/selectediipath.cpp \
    src/cevalconfig.cpp \
    src/gui/ediiconnectionfaileddialog.cpp \
    src/softwareupdateresult.cpp

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
    src/comboboxmodel.h \
    src/evaluationwidgetconnector.h \
    src/evalmainwindowconnector.h \
    src/dynamiccomboboxmodel.h \
    src/gui/evaluatedpeakswidget.h \
    src/helpers.h \
    src/gui/selectpeakdialog.h \
    src/foundpeaksmodel.h \
    src/math/extremesearching.h \
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
    src/plotcontext.h \
    src/plotcontextlimited.h \
    src/evaluatedpeaksmodel.h \
    src/evaluatedpeakswidgetconnector.h \
    src/math/regressor/regress.h \
    src/math/matrix/core.h \
    src/math/matrix/core.hpp \
    src/math/matrix/gauss/gauss.h \
    src/math/matrix/gauss/gauss.hpp \
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
    src/commonparameterswidgetconnector.h \
    src/hvlcalculator.h \
    src/stringmappermodel.h \
    src/integermappermodel.h \
    src/plotcontexttypes.h \
    src/maincontrolswidgetconnector.h \
    src/gui/addpeakdialog.h \
    src/evalserializable.h \
    src/globals.h \
    src/math/regressor/profiler.h \
    src/doubletostringconvertor.h \
    src/gui/numberformatdialog.h \
    src/math/regressor/hvlPeak.h \
    src/gui/hvlfitinprogressdialog.h \
    src/witchcraft.h \
    src/mappedvectorwrapper.h \
    src/crashhandling/crashhandlerlinux_stacktrace.h \
    src/crashhandling/crashhandlerwindows_stacktrace.h \
    src/gui/crashhandlerdialog.h \
    src/gui/adjustplotvisualsdialog.h \
    src/custommetatypes.h \
    src/standardplotcontextsettingshandler.h \
    src/serieproperties.h \
    src/math/mystd/foostream.h \
    src/gui/aboutdialog.h \
    src/scrollareaeventfilter.h \
    src/inumberformatchangeable.h \
    src/gui/evaluationupperwidget.h \
    src/evaluationupperwidgetconnector.h \
    src/gui/setaxistitlesdialog.h \
    src/gui/exportdatatabletocsvdialog.h \
    src/plotexporter.h \
    src/gui/exportplottoimagedialog.h \
    src/abstractpeakfinder.h \
    src/assistedpeakfinder.h \
    src/manualpeakfinder.h \
    src/ploteventfilter.h \
    src/gui/common/collapseexpandbutton.h \
    src/gui/common/collapsiblegroupbox.h \
    src/gui/softwareupdatewidget.h \
    src/gui/autoupdatecheckdialog.h \
    src/gui/checkforupdatedialog.h \
    src/softwareupdater.h \
    src/doubleclickableqwtplotzoomer.h \
    src/dataexporter/exporter.h \
    src/dataexporter/exporterelems.h \
    src/dataexporter/exporterglobals.h \
    src/dataexporter/schemeserializer.h \
    src/dataexporter/backends/abstractexporterbackend.h \
    src/dataexporter/backends/textexporterbackend.h \
    src/dataexporter/backends/htmlexporterbackend.h \
    src/dataexporter/schemesmanagerwidget.h \
    src/dataexporter/schemeeditor.h \
    src/gui/textexporterbackendconfigurationdialog.h \
    src/dataexporter/backends/textstreamexporterbackend.h \
    src/gui/appendoverwriteexportfilemessagebox.h \
    src/hvllibwrapper.h \
    src/gui/specifypeakboundariesdialog.h \
    src/gui/hyperbolafitwidget.h \
    src/hyperbolafititems.h \
    src/hyperbolafittingengine.h \
    src/hyperbolafittingenginemsgs.h \
    src/hyperbolafittingwidgetconnector.h \
    src/math/regressor/hyperbola.h \
    src/math/regressor/hyperbola2.h \
    src/gui/registerinhyperbolafitdialog.h \
    src/gui/registerinhyperbolafitwidget.h \
    src/customlocalesortproxymodel.h \
    src/crashhandling/crashhandlerwindows.h \
    src/crashhandling/crashhandlerbase.h \
    src/crashhandling/nullcrashhandler.h \
    src/crashevent.h \
    src/crashhandling/crashhandlerfinalizer.h \
    src/crashhandling/crashhandlingprovider.h \
    src/cevalcrashhandler.h \
    src/crasheventcatcher.h \
    src/crashhandling/crashhandlerlinux.h \
    src/crashhandling/rawmemblock.h \
    src/gui/hvlestimateinprogressdialog.h \
    src/efg/efgloaderwatcher.h \
    src/efg/efgloaderinterface.h \
    src/efg/efgtypes.h \
    src/efg/ipcclient.h \
    src/efg/localsocketclient.h \
    src/gui/efgloadinfo.h \
    src/hvlextrapolator.h \
    src/snrcalculator.h \
    src/datahash.h \
    src/filescomboboxmodel.h \
    src/gui/ediinotfounddialog.h \
    src/gui/selectediipath.h \
    src/cevalconfig.h \
    src/gui/ediiconnectionfaileddialog.h \
    src/softwareupdateresult.h

ipc_dbus {
    QT += dbus

    DEFINES += ENABLE_IPC_INTERFACE_DBUS

    SOURCES += \
        src/efg/dbusclient.cpp \

    HEADERS += \
        src/efg/dbusclient.h \
}

FORMS    += src/gui/evalmainwindow.ui \
    src/gui/maincontrolswidget.ui \
    src/gui/evaluationwidget.ui \
    src/gui/commonparameterswidget.ui \
    src/gui/loadchemstationdatadialog.ui \
    src/gui/evaluatedpeakswidget.ui \
    src/gui/selectpeakdialog.ui \
    src/gui/addpeakdialog.ui \
    src/gui/numberformatdialog.ui \
    src/gui/hvlfitinprogressdialog.ui \
    src/gui/crashhandlerdialog.ui \
    src/gui/adjustplotvisualsdialog.ui \
    src/gui/aboutdialog.ui \
    src/gui/evaluationupperwidget.ui \
    src/gui/setaxistitlesdialog.ui \
    src/gui/exportdatatabletocsvdialog.ui \
    src/gui/exportplottoimagedialog.ui \
    src/gui/softwareupdatewidget.ui \
    src/gui/autoupdatecheckdialog.ui \
    src/gui/checkforupdatedialog.ui \
    src/dataexporter/schemesmanagerwidget.ui \
    src/dataexporter/schemeeditor.ui \
    src/gui/textexporterbackendconfigurationdialog.ui \
    src/gui/specifypeakboundariesdialog.ui \
    src/gui/hyperbolafitwidget.ui \
    src/gui/registerinhyperbolafitdialog.ui \
    src/gui/registerinhyperbolafitwidget.ui \
    src/gui/hvlestimateinprogressdialog.ui \
    src/gui/ediinotfounddialog.ui \
    src/gui/selectediipath.ui \
    src/gui/ediiconnectionfaileddialog.ui

unix {
    LIBS += -ldl
} win32 {
    LIBS += -ldbghelp
}

include($$PWD/CEval.pri)

!win32-msvc2015 {
    QMAKE_CXXFLAGS += "-Wall -Wextra -pedantic -isystem \"$$BOOSTPATH\" -isystem \"$$EIGENPATH\" -isystem \"$$QWTPATH/include\""
} else {
    QMAKE_CXXFLAGS_RELEASE += /Zi /GL
    QMAKE_LFLAGS_RELEASE += /MAP /LTCG /debug /opt:ref
    QMAKE_CXXFLAGS += /openmp

    win32_i386: QMAKE_CXXFLAGS += /arch:SSE2

    INCLUDEPATH += $$BOOSTPATH
}

gcc {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
    greaterThan(COMPILER_MAJOR_VERSION, 9) : !CONFIG(xsanitize) {
        CONFIG += use_lto
    }
}

clang {
    COMPILER_VERSION_STRING_FULL = $$system($$QMAKE_CXX " --version")
    COMPILER_VERSION_STRING = $$split(COMPILER_VERSION_STRING_FULL, " ")
    COMPILER_VERSION_PART = $$member(COMPILER_VERSION_STRING, 2, 2)
    COMPILER_VERSION_LIST = $$split(COMPILER_VERSION_PART, .)
    COMPILER_VERSION_MAJOR = $$first(COMPILER_VERSION_LIST)

    greaterThan(COMPILER_VERSION_MAJOR, 9) : !CONFIG(xsanitize) {
        CONFIG += use_lto
    }
}

use_lto {
    message("LTO enabled")
    QMAKE_CXXFLAGS += -flto

    clang {
        CONFIG(debug, debug|release): QMAKE_CXXFLAGS += -O0
        CONFIG(release, debug|release): QMAKE_CXXFLAGS += -O3
    }
}

RESOURCES += \
    cevalres.qrc

win32 {
    RC_FILE = CEval.rc
    CONFIG += embed_manifest_exe
}

flatpak_build {
    DEFINES += CEVAL_FLATPAK_BUILD

    package.path = /app/bin
    package.files += CEval
    package.CONFIG = no_check_exist
    INSTALLS += package
}
