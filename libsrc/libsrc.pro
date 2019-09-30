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

PRJDIR = ".."

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
DIST_HEADERS += \
    intfRESTAPIHolder.h \
    QRESTServer.h \
    HTTPExceptions.h \
    GenericTypes.h

PRIVATE_HEADERS += \
    Private/clsRequestHandler.h \
    Private/Configs.hpp \
    Private/RESTAPIRegistry.h \
    Private/clsAPIObject.hpp \
    Private/intfAPIArgManipulator.h \
    Private/APICache.hpp \
    Private/intfCacheConnector.hpp \
    Private/clsRedisConnector.h \
    Private/WebSocketServer.hpp \
    Private/QJWT.h \
    Private/clsSimpleCrypt.h \


# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    libID.cpp \
    Private/clsRequestHandler.cpp \
    Private/RESTAPIRegistry.cpp \
    QRESTServer.cpp \
    Private/clsRedisConnector.cpp \
    Private/QJWT.cpp \
    Private/clsSimpleCrypt.cpp \
    Private/GenericTypes.cpp

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
OTHER_FILES += \

################################################################################
#                       DO NOT CHANGE ANYTHING BELOW                           #
################################################################################

ConfigFile = $$PRJDIR/qmake/configs.pri
!exists($$ConfigFile){
error("**** libsrc: Unable to find Configuration file $$ConfigFile ****")
}
include ($$ConfigFile)

TEMPLATE = lib
win32:DEFINES *= QHTTP_EXPORT
TARGET = $$ProjectName

DESTDIR      = $$BaseLibraryFolder
MOC_DIR      = $$BuildFolderPattern/$$TARGET/moc
OBJECTS_DIR  = $$BuildFolderPattern/$$TARGET/obj
RCC_DIR      = $$BuildFolderPattern/$$TARGET/rcc
UI_DIR       = $$BuildFolderPattern/$$TARGET/ui

QMAKE_CXXFLAGS_RELEASE += -fPIC
QMAKE_CXXFLAGS_DEBUG += -fPIC

build_static {
    DEFINES += TARGOMAN_BUILD_STATIC
    CONFIG+= staticlib
}

HEADERS += $$DIST_HEADERS \
           $$PRIVATE_HEADERS \
           $$SUBMODULE_HEADERS \

include($$PRJDIR/qmake/install.pri)
