#/*******************************************************************************
# * FastRESTServer a lean and mean Qt/C++ based REST server                     *
# *                                                                             *
# * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
# *                                                                             *
# *                                                                             *
# * FastRESTServer is free software: you can redistribute it and/or modify      *
# * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
# * the Free Software Foundation, either version 3 of the License, or           *
# * (at your option) any later version.                                         *
# *                                                                             *
# * FastRESTServer is distributed in the hope that it will be useful,           *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
# * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
# * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
# * along with FastRESTServer. If not, see <http://www.gnu.org/licenses/>.      *
# *                                                                             *
# *******************************************************************************/
ProjectName="QRESTServer"
VERSION=1.0.0

#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-
# Qt5.5.1 on OSX needs both c++11 and c++14!! the c++14 is not enough
QMAKE_CXXFLAGS += -std=c++11 -std=c++14
CONFIGS += c++11 c++14

QT += core network
QT -= gui

LIBS += -lTargomanCommon \
        -lQFieldValidator \
        -lqhttp

EXTERNAL_DEPS=1
