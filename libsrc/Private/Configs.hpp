/*******************************************************************************
 * QRESTServer a lean and mean Qt/C++ based REST server                     *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * QRESTServer is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * QRESTServer is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
 * along with QRESTServer. If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                             *
 *******************************************************************************/
/**
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#ifndef QHTTP_PRIVATE_CONFIGS_HPP
#define QHTTP_PRIVATE_CONFIGS_HPP

#include <QTimer>
#include <QMap>

#include "libTargomanCommon/Macros.h"
#include "QHttp/QHttpServer"

#include "libTargomanCommon/exTargomanBase.h"
#include "intfRESTAPIHolder.h"
#include "QRESTServer.h"

namespace QHttp {
namespace Private {

struct stuConfigs
{
    RESTServer::stuConfig Public;
    struct stuPrivate{
        QString BaseParthWithVersion;
        bool IsStarted = false;
    } Private;
};

extern stuConfigs gConfigs;
extern stuStatistics gServerStats;

class clsAPIObject;

extern QList<intfAPIArgManipulator*> gOrderedMetaTypeInfo;
extern QList<intfAPIArgManipulator*> gUserDefinedTypesInfo;

}
}

#endif // QHTTP_PRIVATE_CONFIGS_HPP
