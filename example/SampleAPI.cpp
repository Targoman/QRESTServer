/*******************************************************************************
 * QRESTServer a lean and mean Qt/C++ based REST server                        *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * QRESTServer is free software: you can redistribute it and/or modify         *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * QRESTServer is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
 * along with QRESTServer. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                             *
 *******************************************************************************/
/**
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#include "SampleAPI.h"
#include "libTargomanCommon/CmdIO.h"
#include "QHttp/QRESTServer.h"


namespace ns {

using namespace QHttp;
SampleAPI::SampleAPI()
{
    TargomanDebug(1, "initialization");

    QHTTP_REGISTER_TARGOMAN_ENUM(ns::enuSample);
    QHTTP_REGISTER_TARGOMAN_ENUM(ns::enuSample2);

    this->registerMyRESTAPIs();

    SampleSubModule::instance().init();

}

void SampleAPI::init()
{
}

CACHEABLE_3H int SampleAPI::apiGETSampleData()
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ()");
    return 5;
}

COOKIES_t SampleAPI::apiGETSampleDataWithCookie(COOKIES_t _COOKIES, QHttp::ExtraPath_t _EXTRAPATH)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_COOKIES.toVariant().toString()<<","<<_EXTRAPATH<<")");
    return _COOKIES;
}

HEADERS_t SampleAPI::apiGETSampleDataWithHeaders(HEADERS_t _HEADERS)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_HEADERS.toVariant().toString()<<")");
    return _HEADERS;
}

QString SampleAPI::apiGETSampleDataReturningJWT()
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ()");
    return this->createSignedJWT(QJsonObject({{"a",1},{"b","fdsdfdf"}}), QJsonObject({{"priv1",4}}), 120, "fdffsdf");
}

EncodedJWT_t SampleAPI::asyncApiGETSampleDataWithJWT(JWT_t _JWT)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<QJsonDocument(_JWT).toJson()<<")");
    return createSignedJWT(_JWT);
}

EncodedJWT_t SampleAPI::asyncApiGETSampleDataWithJWTAndRemoteIP(QHttp::JWT_t _JWT, QHttp::RemoteIP_t _ip, int sample)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<QJsonDocument(_JWT).toJson()<<")");
    return createSignedJWT(_JWT);
}

int SampleAPI::apiPUTSampleData(quint64 _id, const QString& _info)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: (\""<<_id<<"\",\""<<_info<<"\")");
    return _id;
}

int SampleAPI::apiDELETESampleData(QHttp::ExtraPath_t _EXTRAPATH)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_EXTRAPATH<<")");
    return 3;
}

QVariantList SampleAPI::apiUPDATESampleData(char _id, const QString& _info)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_id<<","<<_info<<")");
    return QVariantList({{1},{"sdjkfh"}});
}

stuTable SampleAPI::apiTranslate(const QString& _text, QString _detailed)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: (\""<<_text<<"\",\""<<_detailed<<"\")");
    return stuTable();
}


stuTable SampleAPI::apiSampleList(const QVariantList& _list)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: (_list)");
    return stuTable(_list.size(), {{"data", _list}});
}

QString SampleAPI::apiWSSample(const QString _value)
{
    TargomanDebug(1, "Called: " <<__FUNCTION__<<" Params: ("<<_value<<")");
    return _value;
}

void SampleSubModule::init()
{

}

QString SampleSubModule::apiGET(QHttp::ExtraPath_t _EXTRAPATH, int from, int to)
{
    return _EXTRAPATH;
}

SampleSubModule::SampleSubModule()
{
    this->registerMyRESTAPIs();
}

quint64 SampleSubModule::apiSampleEnahancedEnum (ns::enuSample::Type _val){
    return _val;
}

quint64 SampleSubModule::apiSampleTargomanEnum (ns::enuSample2::Type _val, QString A){
    return _val;
}

}

