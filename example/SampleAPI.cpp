/*******************************************************************************
 * FastRESTServer a lean and mean Qt/C++ based REST server                     *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * FastRESTServer is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * FastRESTServer is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
 * along with FastRESTServer. If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                             *
 *******************************************************************************/
/**
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#include "SampleAPI.h"
#include "libTargomanCommon/CmdIO.h"

namespace Sample1 {

using namespace QHttp;
SampleAPI::SampleAPI()
{
    TargomanDebug(1, "initialization");
    this->registerMyRESTAPIs();
}

void SampleAPI::init()
{
}

int SampleAPI::apiGETSampleData()
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ()");
    return 5;
}

int SampleAPI::apiPUTSampleData(quint64 _id, const QString &_info)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: (\""<<_id<<"\",\""<<_info<<"\")");
    return _id;
}

int SampleAPI::apiDELETESampleData(quint64 _id)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_id<<")");
    return 3;
}

int SampleAPI::apiUPDATESampleData(quint64 _id, const QString& _info)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_id<<","<<_info<<")");
    return 2;
}

stuTable SampleAPI::apiTranslate(const QString &_text, bool _detailed)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_text<<","<<_detailed<<")");
    return stuTable();
}

}
