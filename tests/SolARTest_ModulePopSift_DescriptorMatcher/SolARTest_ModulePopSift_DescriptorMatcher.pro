## remove Qt dependencies
QT       -= core gui
CONFIG -= qt

## global defintions : target lib name, version
TARGET = SolARTest_ModulePopSift_DescriptorMatcher
VERSION=0.9.3

DEFINES += MYVERSION=$${VERSION}
CONFIG += c++1z
CONFIG += console

include(findremakenrules.pri)

CONFIG(debug,debug|release) {
    TARGETDEPLOYDIR = $${PWD}/../bin/Debug
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    TARGETDEPLOYDIR = $${PWD}/../bin/Release
    DEFINES += _NDEBUG=1
    DEFINES += NDEBUG=1
}

DEPENDENCIESCONFIG = sharedlib install_recurse

win32:CONFIG -= static
win32:CONFIG += shared

## Configuration for Visual Studio to install binaries and dependencies. Work also for QT Creator by replacing QMAKE_INSTALL
PROJECTCONFIG = QTVS

#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include ($$shell_quote($$shell_path($${QMAKE_REMAKEN_RULES_ROOT}/templateappconfig.pri)))  # Shell_quote & shell_path required for visual on windows

#DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK
DEFINES += BOOST_AUTO_LINK_NOMANGLE
DEFINES += BOOST_LOG_DYN_LINK

SOURCES += \
    main.cpp

unix {
    LIBS += -ldl
    QMAKE_CXXFLAGS += -DBOOST_ALL_DYN_LINK
}

macx {
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
}

android {
    ANDROID_ABIS="arm64-v8a"
}

linux {
  run_install.path = $${TARGETDEPLOYDIR}
  run_install.files = $${PWD}/../run.sh
  CONFIG(release,debug|release) {
    run_install.extra = cp $$files($${PWD}/../runRelease.sh) $${PWD}/../run.sh
  }
  CONFIG(debug,debug|release) {
    run_install.extra = cp $$files($${PWD}/../runDebug.sh) $${PWD}/../run.sh
  }
  INSTALLS += run_install
}

configfile.path = $${TARGETDEPLOYDIR}/
configfile.files = $${PWD}/SolARTest_ModulePopSift_DescriptorMatcher_conf.xml
INSTALLS += configfile

DISTFILES += \
    packagedependencies.txt

#NOTE : Must be placed at the end of the .pro
include ($$shell_quote($$shell_path($${QMAKE_REMAKEN_RULES_ROOT}/remaken_install_target.pri)))) # Shell_quote & shell_path required for visual on windows
