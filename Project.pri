#/*******************************************************************************
# * QRESTServer a lean and mean Qt/C++ based REST server                        *
# *                                                                             *
# * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
# *                                                                             *
# *                                                                             *
# * QRESTServer is free software: you can redistribute it and/or modify         *
# * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
# * the Free Software Foundation, either version 3 of the License, or           *
# * (at your option) any later version.                                         *
# *                                                                             *
# * QRESTServer is distributed in the hope that it will be useful,              *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
# * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
# * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
# * along with QRESTServer. If not, see <http://www.gnu.org/licenses/>.         *
# *                                                                             *
# *******************************************************************************/
ProjectDependencies +=
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

################################################################################
#                     DO NOT CHANGE ANYTHING BELOW
################################################################################

!unix{
  error("********* Compile on OS other than Linux is not ready yet")
}

isEmpty(PREFIX) {
 PREFIX = $$(HOME)/local
}

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
isEmpty(PRJDIR) {
 PRJDIR = .
}
BaseOutput=$$PRJDIR/$$BaseOutput
message("*********************   $$ProjectName CONFIG  ********************** ")
message("* Building $$ProjectName Ver. $$VERSION")
message("* Base Out Path has been set to: $$BaseOutput/")
message("* Install Path has been set to: $$PREFIX/")
message("****************************************************************** ")

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

BaseLibraryFolder        = $$BaseOutput/out/$$LibFolderPattern
BaseLibraryIncludeFolder = $$BaseOutput/out/$$LibIncludeFolderPattern
BaseBinFolder            = $$BaseOutput/out/$$BinFolderPattern
BaseTestBinFolder        = $$BaseOutput/out/$$TestBinFolder
BaseUnitTestBinFolder    = $$BaseOutput/out/$$UnitTestBinFolder
BaseBuildFolder          = $$BaseOutput/out/$$BuildFolderPattern/$$ProjectName
BaseConfigFolder         = $$BaseOutput/out/$$ConfigFolderPattern

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
INCLUDEPATH += $$BaseLibraryIncludeFolder \
               $$PREFIX/include \
               $$DependencyIncludePaths/

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
DependencyLibPaths      +=   $$BaseLibraryFolder $$PREFIX/lib $$PREFIX/lib64
FullDependencySearchPaths=   $$DependencyLibPaths

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

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
unix{
  documentation.path = $$PREFIX/doc
  documentation.files=docs/*
#  documentation.extra=create_docs; mv master.doc toc.doc

  target.files= $$BaseOutput/out/$$BinFolderPattern \
                $$BaseOutput/out/$$LibFolderPattern \
                $$BaseOutput/out/$$ConfigFolderPattern

  target.path = $$PREFIX/
  target.extra= rm -rvf $$PREFIX/lib/lib/; mkdir -p $$PREFIX/$$LibIncludeFolderPattern/ $$BaseOutput/out/$$LibIncludeFolderPattern; cp -Lr $$BaseOutput/out/$$LibIncludeFolderPattern $$PREFIX/$$LibIncludeFolderPattern/..;

  INSTALLS += documentation \
              target
}

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
defineTest(addSubdirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}

QMAKE_CXXFLAGS += -std=c++14
CONFIGS += c++14

