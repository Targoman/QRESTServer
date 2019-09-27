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
#include <Private/intfAPIArgManipulator.h>

#include "GenericTypes.h"
#include "HTTPExceptions.h"
#include <QRegularExpression>
#include "Private/Configs.hpp"

namespace QHttp {

using namespace Private;

void registerGenericTypes()
{
    static QRegularExpression rxMd5("", QRegularExpression::CaseInsensitiveOption);

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
        QHttp::RemoteIP_t,
        [](const QHttp::RemoteIP_t& _value) -> QVariant {return _value;},
        [](const QVariant& _value, const QByteArray&) -> QHttp::RemoteIP_t {QHttp::RemoteIP_t Value;Value=_value.toString();return  Value;}
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::MD5_t,
        [](const QHttp::MD5_t& _value) -> QVariant {return _value;},
        [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::MD5_t {
            QHttp::MD5_t Value;
            if(rxMd5.match(_value.toString()).hasMatch() == false)
                throw exHTTPBadRequest("Invalid MD5");
            Value=_value.toString();  return  Value;
        }
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::Email_t,
        [](const QHttp::Email_t& _value) -> QVariant {return _value;},
        [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::Email_t {
            QHttp::Email_t Value;
            if(rxEmail.match(_value.toString()).hasMatch() == false)
                throw exHTTPBadRequest("Invalid Email");
            Value=_value.toString();  return  Value;
        }
    );

    QHTTP_REGISTER_METATYPE(
        QHttp::JWT_t,
        [](const QHttp::JWT_t& _value) -> QVariant {
            return _value.toVariantMap();
        },
        [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::JWT_t {
            QJsonObject Obj;
            if(_value.canConvert<QVariantMap>())
                Obj = Obj.fromVariantMap(_value.value<QVariantMap>());
            if(_value.canConvert<QVariantHash>())
                Obj = Obj.fromVariantHash(_value.value<QVariantHash>());

            if(Obj.isEmpty())
                throw exHTTPBadRequest("Unable to convert JWT");
            return  *reinterpret_cast<QHttp::JWT_t*>(&Obj);
        }
    );

}

}
