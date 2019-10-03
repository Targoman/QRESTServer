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
 * @author S. Mehran M. Ziabary <ziabary@targoman.com>
 */
#include <QRegularExpression>
#include "intfAPIArgManipulator.h"
#include "GenericTypes.h"
#include "HTTPExceptions.h"
#include "Private/Configs.hpp"
#include "QFieldValidator.h"

namespace QHttp {

using namespace Private;

static bool Registered = false;

void registerGenericTypes()
{
    if(Registered)
        return;

    Registered = true;

    QHTTP_REGISTER_METATYPE(
        QHttp::stuTable,
        [](const QHttp::stuTable& _value) -> QVariant{return QVariantMap({{"totalRows", _value.TotalRows}, {"Rows", _value.Rows}});},
        nullptr
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::COOKIES_t,
        [](const QHttp::COOKIES_t& _value) -> QVariant {return _value.toVariant();},
        [](const QVariant& _value, const QByteArray&) -> QHttp::COOKIES_t {QHttp::COOKIES_t  TempValue;return TempValue.fromVariant(_value);}
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::HEADERS_t,
        [](const QHttp::HEADERS_t& _value) -> QVariant {return _value.toVariant();},
        [](const QVariant& _value, const QByteArray&) -> QHttp::HEADERS_t {QHttp::HEADERS_t  TempValue;return TempValue.fromVariant(_value);}
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::JWT_t,
        nullptr,
        [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::JWT_t {
            QJsonObject Obj;
            if(_value.canConvert<QVariantMap>())
                Obj = Obj.fromVariantMap(_value.value<QVariantMap>());

            if(Obj.isEmpty())
                throw exHTTPBadRequest(_paramName + " is not a valid JWT object");
            return  *reinterpret_cast<QHttp::JWT_t*>(&Obj);
        }
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::EncodedJWT_t,
        [](const QHttp::EncodedJWT_t& _value) -> QVariant {return _value;},
        nullptr
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::RemoteIP_t,
        [](const QHttp::RemoteIP_t& _value) -> QVariant {return _value;},
        [](const QVariant& _value, const QByteArray&) -> QHttp::RemoteIP_t {QHttp::RemoteIP_t Value;Value=_value.toString();return  Value;}
    );




    //QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(QHttp::EncodedJWT_t, allwaysInvalid(), _value);
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(QHttp::JSON_t, optional(QFieldValidator().json()), _value);
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(QHttp::MD5_t, optional(QFieldValidator().md5()), _value);
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(QHttp::Email_t, optional(QFieldValidator().email()), _value);
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(QHttp::Mobile_t, optional(QFieldValidator().mobile()), _value);


}

}
