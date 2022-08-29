cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(AnglerFish LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLATPAK_BUILD "Build application as a Flatpak package" OFF)
option(BUILD_AS_DEVELOPMENT "Build as development version" OFF)

set(QWT_DIR "" CACHE PATH "Path to Qwt library installation")
set(EIGEN_DIR "" CACHE PATH "Path to Eigen library installation")
set(EDII_DIR "" CACHE PATH "Path to EDII package installation")
set(ECHMET_UPDATER_DIR "" CACHE PATH "Path to ECHMETUpdater library installation")

if (WIN32)
    if (MINGW OR MSYS)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
            message(FATAL_ERROR "MinGW 10.0 or newer is required to build AnglerFish")
        endif ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-function -ftree-vectorize -msse2 -g -fopenmp")
        add_definitions("-DECHMET_COMPILER_MINGW")
        set(DEFAULT_SYMVER_LINK "-Wl,--default-symver")
    elseif (MSVC)
        add_definitions("-DECHMET_COMPILER_MSVC")
        set(DEFAULT_SYMVER_LINK " ")
    else ()
        message(FATAL_ERROR "Misdetected or unsupported compiler")
    endif ()
elseif (UNIX)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
            message(FATAL_ERROR "GCC 10.0 or newer is required to build AnglerFish")
        elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND
                CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
            message(FATAL_ERROR "Clang 8.0 or newer is required to build AnglerFish")
        endif ()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -flto -fopenmp -g -pipe")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
        add_definitions("-DECHMET_COMPILER_GCC_LIKE")
    endif ()
else ()
    message(FATAL_ERROR "Platform not recognized or not supported")
endif ()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup()

find_library(
    LIBQWT
    qwt
    PATHS "${QWT_DIR}/lib"
    REGISTRY_VIEW TARGET REQUIRED
)
find_library(
    LIBECHMETUPDATECHECK
    ECHMETUpdateCheck PATHS "${ECHMET_UPDATER_DIR}/lib"
    REGISTRY_VIEW TARGET REQUIRED
)

include_directories(SYSTEM "${QWT_DIR}/include")
include_directories(SYSTEM "${EIGEN_DIR}")
include_directories(SYSTEM "${EDII_DIR}/include")
include_directories(SYSTEM "${ECHMET_UPDATER_DIR}/include")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions("-DQWT_DLL")

if (FLATPAK_BUILD)
    add_definitions("-DCEVAL_FLATPAK_BUILD")
endif ()
if (BUILD_AS_DEVELOPMENT)
    add_definitions("-DUNSTABLE_VERSION")
endif ()

set(CEVAL_SRCS
    "src/main.cpp"
    "src/evaluationenginecommonparametersview.cpp"
    "src/gui/evalmainwindow.cpp"
    "src/gui/common/floatingvaluelineedit.cpp"
    "src/commonparametersengine.cpp"
    "src/dataaccumulator.cpp"
    "src/floatingvaluedelegate.cpp"
    "src/evaluationengine.cpp"
    "src/gui/common/qmappedcheckbox.cpp"
    "src/gui/maincontrolswidget.cpp"
    "src/gui/evaluationwidget.cpp"
    "src/gui/commonparameterswidget.cpp"
    "src/evaluationwidgetconnector.cpp"
    "src/evalmainwindowconnector.cpp"
    "src/gui/evaluatedpeakswidget.cpp"
    "src/gui/parameterscarryoverdialog.cpp"
    "src/gui/selectpeakdialog.cpp"
    "src/foundpeaksmodel.cpp"
    "src/math/extremesearching.cpp"
    "src/peakevaluator.cpp"
    "src/math/functions.cpp"
    "src/math/alglib/ap.cpp"
    "src/math/alglib/chisquaredistr.cpp"
    "src/math/alglib/gammaf.cpp"
    "src/math/alglib/ibetaf.cpp"
    "src/math/alglib/igammaf.cpp"
    "src/math/alglib/normaldistr.cpp"
    "src/math/alglib/studenttdistr.cpp"
    "src/plotcontext.cpp"
    "src/plotcontextlimited.cpp"
    "src/evaluatedpeaksmodel.cpp"
    "src/evaluatedpeakswidgetconnector.cpp"
    "src/commonparameterswidgetconnector.cpp"
    "src/helpers.cpp"
    "src/hvlcalculator.cpp"
    "src/maincontrolswidgetconnector.cpp"
    "src/gui/addpeakdialog.cpp"
    "src/evalserializable.cpp"
    "src/globals.cpp"
    "src/doubletostringconvertor.cpp"
    "src/gui/numberformatdialog.cpp"
    "src/gui/hvlfitinprogressdialog.cpp"
    "src/witchcraft.cpp"
    "src/gui/crashhandlerdialog.cpp"
    "src/gui/adjustplotvisualsdialog.cpp"
    "src/custommetatypes.cpp"
    "src/standardplotcontextsettingshandler.cpp"
    "src/serieproperties.cpp"
    "src/gui/aboutdialog.cpp"
    "src/scrollareaeventfilter.cpp"
    "src/gui/evaluationupperwidget.cpp"
    "src/evaluationupperwidgetconnector.cpp"
    "src/gui/setaxistitlesdialog.cpp"
    "src/gui/exportdatatabletocsvdialog.cpp"
    "src/plotexporter.cpp"
    "src/gui/exportplottoimagedialog.cpp"
    "src/abstractpeakfinder.cpp"
    "src/assistedpeakfinder.cpp"
    "src/manualpeakfinder.cpp"
    "src/ploteventfilter.cpp"
    "src/peakcontext.cpp"
    "src/gui/common/collapseexpandbutton.cpp"
    "src/gui/common/collapsiblegroupbox.cpp"
    "src/gui/softwareupdatewidget.cpp"
    "src/gui/autoupdatecheckdialog.cpp"
    "src/gui/checkforupdatedialog.cpp"
    "src/softwareupdater.cpp"
    "src/evalmainwindowsoftwareupdaterconnector.cpp"
    "src/doubleclickableqwtplotzoomer.cpp"
    "src/evaluationenginedataexporter.cpp"
    "src/dataexporter/exporterelems.cpp"
    "src/dataexporter/exporter.cpp"
    "src/dataexporter/schemeserializer.cpp"
    "src/dataexporter/backends/abstractexporterbackend.cpp"
    "src/dataexporter/backends/textexporterbackend.cpp"
    "src/dataexporter/backends/htmlexporterbackend.cpp"
    "src/dataexporter/schemesmanagerwidget.cpp"
    "src/dataexporter/schemeeditor.cpp"
    "src/gui/textexporterbackendconfigurationdialog.cpp"
    "src/dataexporter/backends/textstreamexporterbackend.cpp"
    "src/gui/appendoverwriteexportfilemessagebox.cpp"
    "src/gui/specifypeakboundariesdialog.cpp"
    "src/hyperbolafittingengine.cpp"
    "src/gui/hyperbolafitwidget.cpp"
    "src/gui/registerinhyperbolafitdialog.cpp"
    "src/gui/registerinhyperbolafitwidget.cpp"
    "src/hyperbolafittingwidgetconnector.cpp"
    "src/customlocalesortproxymodel.cpp"
    "src/crashhandling/crashhandlerwindows.cpp"
    "src/crashhandling/crashhandlerbase.cpp"
    "src/crashhandling/nullcrashhandler.cpp"
    "src/crashevent.cpp"
    "src/cevalcrashhandler.cpp"
    "src/crasheventcatcher.cpp"
    "src/gui/hvlestimateinprogressdialog.cpp"
    "src/math/hvlestimate.cpp"
    "src/efg/efgloaderwatcher.cpp"
    "src/efg/efgloaderinterface.cpp"
    "src/efg/efgtypes.cpp"
    "src/efg/ipcclient.cpp"
    "src/efg/localsocketclient.cpp"
    "src/hvlextrapolator.cpp"
    "src/snrcalculator.cpp"
    "src/datahash.cpp"
    "src/gui/ediinotfounddialog.cpp"
    "src/gui/selectediipath.cpp"
    "src/cevalconfig.cpp"
    "src/gui/ediiconnectionfaileddialog.cpp"
    "src/softwareupdateresult.cpp"
)

# Headers are not part of the source code so we need
# to MOC them manually where required
qt_wrap_cpp(CEVAL_SRCS "src/commonparametersitems.h")
qt_wrap_cpp(CEVAL_SRCS "src/dataaccumulatormsgs.h")
qt_wrap_cpp(CEVAL_SRCS "src/evaluationenginemsgs.h")
qt_wrap_cpp(CEVAL_SRCS "src/evaluationparametersitems.h")
qt_wrap_cpp(CEVAL_SRCS "src/hyperbolafittingenginemsgs.h")
qt_wrap_cpp(CEVAL_SRCS "src/hyperbolafititems.h")
qt_wrap_cpp(CEVAL_SRCS "src/dataexporter/exporterglobals.h")
if (UNIX)
    qt_wrap_cpp(CEVAL_SRCS "src/efg/dbusclient.h")
endif ()

if (WIN32)
    set(CEVAL_LINK_DBUS "")
else (UNIX)
    find_package(Qt6 REQUIRED COMPONENTS DBus)
    set(CEVAL_LINK_DBUS Qt6::DBus)

    add_definitions("-DENABLE_IPC_INTERFACE_DBUS")
    set(CEVAL_SRCS
        "${CEVAL_SRCS}"
        "src/efg/dbusclient.cpp"
    )

    # TODO Check for Linux specifically
    add_definitions("-DCRASHHANDLING_LINUX")
    set(CEVAL_SRCS
        "${CEVAL_SRCS}"
        "src/crashhandling/crashhandlerlinux.cpp"
        "src/crashhandling/crashhandlerlinux_stacktrace.cpp"
        "src/efg/dbusclient.cpp"
    )
endif ()

if (WIN32)
    add_executable(CEval WIN32 ${CEVAL_SRCS})
else ()
    add_executable(CEval ${CEVAL_SRCS})
endif ()

if (WIN32)
    set(CEVAL_LINK_PTHREAD "")
    set(CEVAL_LINK_DBGHELP "dbghelp")
else ()
    set(CEVAL_LINK_PTHREAD "pthread")
    set(CEVAL_LINK_DBGHELP "")
endif ()

target_link_libraries(
    CEval
    PRIVATE ${LIBQWT}
    PRIVATE ${LIBECHMETUPDATECHECK}
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Network
    PRIVATE "${CEVAL_LINK_DBUS}"
    PRIVATE "${CEVAL_LINK_PTHREAD}"
    PRIVATE "${CEVAL_LINK_DBGHELP}"
)

install(TARGETS CEval RUNTIME DESTINATION bin)

if (UNIX)
    install(FILES "CEval.desktop" DESTINATION "share/applications")
    install(FILES "CEval_icon_16.png" DESTINATION "share/icons/hicolor/16x16/apps")
    install(FILES "CEval_icon_32.png" DESTINATION "share/icons/hicolor/32x32/apps")
    install(FILES "CEval_icon_64.png" DESTINATION "share/icons/hicolor/64x64/apps")
endif ()
