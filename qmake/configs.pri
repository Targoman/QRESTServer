include(version.pri)

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
CONFIG(debug, debug|release): DEFINES += TARGOMAN_SHOW_DEBUG=1
CONFIG(release){
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}
DEFINES += TARGOMAN_DEBUG_PROCESS_LINE=1
DEFINES += TARGOMAN_SHOW_WARNING=1
DEFINES += TARGOMAN_SHOW_INFO=1
DEFINES += TARGOMAN_SHOW_HAPPY=1
DEFINES += TARGOMAN_SHOW_NORMAL=1

DEFINES += PROJ_VERSION=$$VERSION
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
# Qt5.5.1 on OSX needs both c++11 and c++14!! the c++14 is not enough
CONFIG  += c++11 c++14

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
QT += core network
QT -= gui

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
LibFolderPattern        = ./lib
LibIncludeFolderPattern = ./include
BinFolderPattern        = ./bin
BuildFolderPattern      = ./build
TestBinFolder           = ./test
UnitTestBinFolder       = ./unitTest
ConfigFolderPattern     = ./conf

BaseLibraryFolder        = $$PRJDIR/out/$$LibFolderPattern
BaseLibraryIncludeFolder = $$PRJDIR/out/$$LibIncludeFolderPattern
BaseBinFolder            = $$PRJDIR/out/$$BinFolderPattern
BaseTestBinFolder        = $$PRJDIR/out/$$TestBinFolder
BaseUnitTestBinFolder    = $$PRJDIR/out/$$UnitTestBinFolder
BaseBuildFolder          = $$PRJDIR/out/$$BuildFolderPattern/$$ProjectName
BaseConfigFolder         = $$PRJDIR/out/$$ConfigFolderPattern

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
INCLUDEPATH += $$PRJDIR/src \
               $$PRJDIR/libsrc \
               $$BaseLibraryIncludeFolder \
               $$PREFIX/include \
               $$DependencyIncludePaths/

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
DependencyLibPaths      +=   $$BaseLibraryFolder $$PREFIX/lib $$PREFIX/lib64

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
win32: DEFINES += _WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX

unix {
  DependencySearchPaths +=

  FullDependencySearchPaths+=  $$DependencySearchPaths \
                               /usr/lib \
                               /usr/lib64 \
                               /usr/local/lib \
                               /usr/local/lib64 \
                               /lib/x86_64-linux-gnu
}

QMAKE_LIBDIR +=  $$DependencyLibPaths

DEPS_BUILT = $$BaseBuildFolder/.depsBuilt

Dependencies.target  = $$DEPS_BUILT
Dependencies.depends = FORCE
unix: Dependencies.commands = $$PRJDIR/buildDependencies.sh $$PRJDIR $$DEPS_BUILT;
win32: error(submodule auto-compile has not yet been implemented for windows)

PRE_TARGETDEPS += $$DEPS_BUILT
QMAKE_EXTRA_TARGETS += Dependencies

HEADERS+= $$VERSIONINGHEADER
