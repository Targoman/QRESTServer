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

PRIVATE_HEADERS += \
    Private/RequestHandler.h \
    Private/Configs.hpp \
    Private/RESTAPIRegistry.h \
    Private/clsAPIObject.hpp

#SUBMODULE_HEADERS += \
#    $$SUBMODULE_TARGOMAN/Macros.h \
#    $$SUBMODULE_TARGOMAN/CmdIO.h \
#    $$SUBMODULE_TARGOMAN/Logger.h \
#    $$SUBMODULE_TARGOMAN/Helpers.hpp \
#    $$SUBMODULE_TARGOMAN/FastOperations.hpp \
#    $$SUBMODULE_TARGOMAN/exTargomanBase.h \
#    $$SUBMODULE_TARGOMAN/clsCountAndSpeed.h \
#    $$SUBMODULE_TARGOMAN/Configuration/tmplConfigurable.h \
#    $$SUBMODULE_TARGOMAN/Configuration/intfConfigurable.hpp \
#    $$SUBMODULE_TARGOMAN/Configuration/ConfigManager.h \
#    $$SUBMODULE_QHTTP/qhttpfwd.hpp \
#    $$SUBMODULE_QHTTP/qhttpabstracts.hpp \
#    $$SUBMODULE_QHTTP/qhttpserverconnection.hpp \
#    $$SUBMODULE_QHTTP/qhttpserverrequest.hpp \
#    $$SUBMODULE_QHTTP/qhttpserverresponse.hpp \
#    $$SUBMODULE_QHTTP/qhttpserver.hpp \
#    $$SUBMODULE_QHTTP/QHttpServer \
#    $$SUBMODULE_QHTTP/private/httpparser.hxx \
#    $$SUBMODULE_QHTTP/private/httpreader.hxx \
#    $$SUBMODULE_QHTTP/private/httpwriter.hxx \
#    $$SUBMODULE_QHTTP/private/qhttpabstractsocket.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpbase.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpclient_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpclientrequest_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpclientresponse_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpserver_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpserverconnection_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpserverrequest_private.hpp \
#    $$SUBMODULE_QHTTP/private/qhttpserverresponse_private.hpp \
#    $$SUBMODULE_QHTTP/../3rdParty/http-parser/http_parser.h \

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    libID.cpp \
    Private/RequestHandler.cpp \
    Private/RESTAPIRegistry.cpp \
    QRESTServer.cpp \

#    $$SUBMODULE_TARGOMAN/CmdIO.cpp \
#    $$SUBMODULE_TARGOMAN/Logger.cpp \
#    $$SUBMODULE_TARGOMAN/clsCountAndSpeed.cpp \
#    $$SUBMODULE_TARGOMAN/exTargomanBase.cpp \
#    $$SUBMODULE_TARGOMAN/Configuration/tmplConfigurable.cpp \
#    $$SUBMODULE_TARGOMAN/Configuration/ConfigManager.cpp \
#    $$SUBMODULE_QHTTP/qhttpabstracts.cpp \
#    $$SUBMODULE_QHTTP/qhttpserverconnection.cpp \
#    $$SUBMODULE_QHTTP/qhttpserverrequest.cpp \
#    $$SUBMODULE_QHTTP/qhttpserverresponse.cpp \
#    $$SUBMODULE_QHTTP/qhttpserver.cpp \
#    $$SUBMODULE_QHTTP/../3rdParty/http-parser/http_parser.c

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
