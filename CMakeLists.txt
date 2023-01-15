cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

if (WIN32 AND MSVC)
    set(CMAKE_SYSTEM_VERSION 8.1 CACHE TYPE INTERNAL FORCE)
endif ()


project(CEval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
    option(FLATPAK_BUILD "Build application as a Flatpak package" OFF)
endif ()
option(BUILD_AS_DEVELOPMENT "Build as development version" OFF)

set(QWT_DIR "" CACHE PATH "Path to Qwt library installation")
set(EIGEN_DIR "" CACHE PATH "Path to Eigen library installation")
set(EDII_DIR "" CACHE PATH "Path to EDII package installation")
set(ECHMETUPDATECHECK_DIR "" CACHE PATH "Path to ECHMETUpdater library installation")
set(BOOST_DIR "" CACHE PATH "Path to Boost headers")

if (UNIX)
    include(GNUInstallDirs)
else ()
    set(CMAKE_INSTALL_LIBDIR "lib")
    set(CMAKE_INSTALL_INCLUDEDIR "include")
    set(CMAKE_INSTALL_BINDIR "bin")
endif ()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)

if (WIN32)
    if (MINGW OR MSYS)
        if (MINGW)
            add_definitions(-DECHMET_COMPILER_MINGW)
        elseif (MSYS)
            add_definitions(-DECHMET_COMPILER_MSYS)
        endif ()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ftree-vectorize -msse2 -g -fopenmp")
    elseif (MSVC)
        add_definitions(-DECHMET_COMPILER_MSVC)
    else ()
        message(FATAL_ERROR "Misdetected or unsupported compiler")
    endif ()

    add_definitions(-DCRASHHANDLING_WIN32)

    set(CEVAL_LINK_DBUS "")
    set(CEVAL_LINK_PTHREAD "")
    set(CEVAL_LINK_DBGHELP "dbghelp")

    if (NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
        add_definitions(-DUSE_CRASHHANDLER)
    endif ()

    if (MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
	endif ()

    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_definitions(-D_WIN32_WINNT=0x0501 -D_USING_V110_SDK71_)

        if (MSVC)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,5.01")
        endif ()
    else ()
        add_definitions(-D_WIN32_WINNT=0x0601)
    endif ()
elseif (UNIX)
    find_package(Qt5 REQUIRED COMPONENTS DBus)

    add_definitions(-DECHMET_COMPILER_GCC_LIKE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ftree-vectorize -msse2 -g -fopenmp")

    set(CEVAL_LINK_DBUS Qt5::DBus)
    set(CEVAL_LINK_PTHREAD "pthread")
    set(CEVAL_LINK_DBGHELP "")

    add_definitions(-DENABLE_IPC_INTERFACE_DBUS)
    set(CEVAL_SRCS
        "${CEVAL_SRCS}"
        "src/efg/dbusclient.cpp"
    )
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        add_definitions(-DCRASHHANDLING_LINUX)
        set(CEVAL_SRCS
            "${CEVAL_SRCS}"
            "src/crashhandling/crashhandlerlinux.cpp"
            "src/crashhandling/crashhandlerlinux_stacktrace.cpp"
            "src/efg/dbusclient.cpp"
        )
    endif ()

    if (NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
        add_definitions(-DUSE_CRASHHANDLER)
        set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -rdynamic)
    endif ()
else ()
    message(FATAL_ERROR "Platform not recognized or not supported")
endif ()

find_library(
    LIBQWT
    qwt
    PATHS
        "${QWT_DIR}/lib"
    REGISTRY_VIEW TARGET REQUIRED
)

# We need to check for both libECHMETUpdateCheck and ECHMETUpdateCheck
# because, depending on what compilers are used to build ECHMETUpdateCheck and CEval,
# the "lib" prefix may be required.
find_library(
    LIBECHMETUPDATECHECK
    ECHMETUpdateCheck
    PATHS
        "${ECHMETUPDATECHECK_DIR}/${CMAKE_INSTALL_LIBDIR}"
    REGISTRY_VIEW TARGET
)
find_library(
    LIBECHMETUPDATECHECK
    libECHMETUpdateCheck
    PATHS
        "${ECHMETUPDATECHECK_DIR}/${CMAKE_INSTALL_LIBDIR}"
    REGISTRY_VIEW TARGET
)

if (${LIBECHMETUPDATECHECK} STREQUAL "LIBECHMETUPDATECHECK-NOTFOUND")
    message(FATAL_ERROR "Cannot find ECHMETUpdateCheck library")
endif ()

if (NOT ("${QWT_DIR}" STREQUAL ""))
    include_directories(SYSTEM "${QWT_DIR}/include")
endif ()

if (NOT ("${EIGEN_DIR}" STREQUAL ""))
    include_directories(SYSTEM "${EIGEN_DIR}")
endif ()

if (NOT ("${EDII_DIR}" STREQUAL ""))
    include_directories(SYSTEM "${EDII_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
endif ()

if (NOT ("${ECHMETUPDATECHECK_DIR}" STREQUAL ""))
    include_directories(SYSTEM "${ECHMETUPDATECHECK_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
endif ()

if (NOT ("${BOOST_DIR}" STREQUAL ""))
    include_directories(SYSTEM "${BOOST_DIR}")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-D_USE_MATH_DEFINES -DQT_DEPRECATED_WARNINGS -DQWT_DLL)

if (FLATPAK_BUILD)
    add_definitions(-DCEVAL_FLATPAK_BUILD)
endif ()
if (BUILD_AS_DEVELOPMENT)
    add_definitions(-DUNSTABLE_VERSION)
else ()
    add_definitions(-DCEVAL_PUBLIC_RELEASE)
endif ()

set(CEVAL_SRCS
    ${CEVAL_SRCS}
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

if (WIN32)
    set(CEVAL_SRCS
        ${CEVAL_SRCS}
        "CEval.rc"
    )
endif ()

# Do not MOC autogenerated files
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_commonparametersitems.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_dataaccumulatormsgs.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_evaluationenginemsgs.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_evaluationparametersitems.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_hyperbolafittingenginemsgs.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/moc_hyperbolafititems.cpp" PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/dataexporter/moc_exporterglobals.cpp" PROPERTY SKIP_AUTOGEN ON)

# Headers are not part of the source code so we need
# to MOC them manually where required
qt5_wrap_cpp(CEVAL_SRCS "src/commonparametersitems.h")
qt5_wrap_cpp(CEVAL_SRCS "src/dataaccumulatormsgs.h")
qt5_wrap_cpp(CEVAL_SRCS "src/evaluationenginemsgs.h")
qt5_wrap_cpp(CEVAL_SRCS "src/evaluationparametersitems.h")
qt5_wrap_cpp(CEVAL_SRCS "src/hyperbolafittingenginemsgs.h")
qt5_wrap_cpp(CEVAL_SRCS "src/hyperbolafititems.h")
qt5_wrap_cpp(CEVAL_SRCS "src/dataexporter/exporterglobals.h")
if (UNIX)
    qt5_wrap_cpp(CEVAL_SRCS "src/efg/dbusclient.h")
endif ()

if (WIN32)
	add_executable(CEval WIN32 ${CEVAL_SRCS})
else ()
	add_executable(CEval ${CEVAL_SRCS})
endif ()

target_link_libraries(
    CEval
    PRIVATE ${LIBQWT}
    PRIVATE ${LIBECHMETUPDATECHECK}
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Network
    PRIVATE ${CEVAL_LINK_DBUS}
    PRIVATE ${CEVAL_LINK_PTHREAD}
    PRIVATE ${CEVAL_LINK_DBGHELP}
)

install(TARGETS CEval RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (UNIX)
    install(FILES "CEval.desktop" DESTINATION "share/applications")
    install(FILES "CEval_icon_16.png" DESTINATION "share/icons/hicolor/16x16/apps")
    install(FILES "CEval_icon_32.png" DESTINATION "share/icons/hicolor/32x32/apps")
    install(FILES "CEval_icon_64.png" DESTINATION "share/icons/hicolor/64x64/apps")
endif ()
