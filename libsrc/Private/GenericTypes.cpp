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
#include "QFieldValidator.h"

namespace QHttp {

using namespace Private;

void registerGenericTypes()
{
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

#define VALIDATION_REQUIRED_TYPE_IMPL(_type, _validationRule, _toVariant) \
    QHTTP_REGISTER_METATYPE( \
        _type, \
        [](const _type& _value) -> QVariant {return _toVariant;}, \
        [](const QVariant& _value, const QByteArray& _paramName) -> _type { \
            static QFieldValidator Validator = QFieldValidator()._validationRule; \
            if(Validator.isValid(_value, _paramName) == false) throw exHTTPBadRequest(Validator.errorMessage()); \
            _type Value; Value=_value.toString();  return  Value; \
        } \
    )

    VALIDATION_REQUIRED_TYPE_IMPL(QHttp::MD5_t, md5(), _value);
    VALIDATION_REQUIRED_TYPE_IMPL(QHttp::Email_t, email(), _value);
    VALIDATION_REQUIRED_TYPE_IMPL(QHttp::Mobile_t, mobile(), _value);

    QHTTP_REGISTER_METATYPE(
        QHttp::JWT_t,
        [](const QHttp::JWT_t& _value) -> QVariant {return _value.toVariantMap();},
        [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::JWT_t {
            static QFieldValidator Validator = QFieldValidator().hasKey("typ",QFieldValidator().equals("JWT")).hasKey("iat");
            if(Validator.isValid(_value, _paramName) == false) throw exHTTPBadRequest(Validator.errorMessage());
            QJsonObject Obj;
            if(_value.canConvert<QVariantMap>())
                Obj = Obj.fromVariantMap(_value.value<QVariantMap>());

            if(Obj.isEmpty())
                throw exHTTPBadRequest(_paramName + " is not a valid JWT object");
            return  *reinterpret_cast<QHttp::JWT_t*>(&Obj);
        }
    );

}

}
