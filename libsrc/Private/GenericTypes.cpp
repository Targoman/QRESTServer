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
                COMPLEXITY_Complex,
                QHttp::stuTable,
                [](const QHttp::stuTable& _value) -> QVariant{return _value.toVariant();},
                nullptr
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_File,
                QHttp::stuFileInfo,
                [](const QHttp::stuFileInfo& _value) -> QVariant{return _value.toVariant();},
                [](const QVariant& _value, const QByteArray& _param) -> QHttp::stuFileInfo {QHttp::stuFileInfo TempValue; return TempValue.fromVariant(_value, _param);}
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_File,
                QHttp::Files_t,
                [](const QHttp::Files_t& _value) -> QVariant{ return _value.toVariant();},
                [](const QVariant& _value, const QByteArray& _param) -> QHttp::Files_t {QHttp::Files_t TempValue; return TempValue.fromVariant(_value, _param);}
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_Complex,
                QHttp::COOKIES_t,
                [](const QHttp::COOKIES_t& _value) -> QVariant {return _value.toVariant();},
                [](const QVariant& _value, const QByteArray&) -> QHttp::COOKIES_t {QHttp::COOKIES_t  TempValue;return TempValue.fromVariant(_value);}
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_Complex,
                QHttp::HEADERS_t,
                [](const QHttp::HEADERS_t& _value) -> QVariant {return _value.toVariant();},
                [](const QVariant& _value, const QByteArray&) -> QHttp::HEADERS_t {QHttp::HEADERS_t  TempValue;return TempValue.fromVariant(_value);}
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_String,
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
                COMPLEXITY_Complex,
                QHttp::ORMFilters_t,
                nullptr,
                [](const QVariant& _value, const QByteArray&) -> QHttp::ORMFilters_t {return _value.toMap();}
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_String,
                QHttp::JSON_t,
                [](const QHttp::JSON_t& _value) -> QVariant {return _value;},
                [](const QVariant& _value, const QByteArray& _paramName) -> QHttp::JSON_t {
                    if(_value.isValid() == false)
                        return QJsonDocument();

                    if(_value.canConvert<QVariantMap>() ||
                       _value.canConvert<QVariantList>() ||
                       _value.canConvert<double>())
                        return QJsonDocument::fromVariant(_value);

                    QJsonParseError Error;
                    QJsonDocument Doc;
                    Doc = Doc.fromJson(_value.toString().toUtf8(), &Error);

                    if(Error.error != QJsonParseError::NoError)
                        throw exHTTPBadRequest(_paramName + " is not a valid Json: <"+_value.toString()+">" + Error.errorString());
                    return  Doc;
                },
                nullptr,
                [](const QList<clsORMField>&){ return "A valid JSON string"; }
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_String,
                QHttp::EncodedJWT_t,
                [](const QHttp::EncodedJWT_t& _value) -> QVariant {return _value;},
                nullptr,
                nullptr,
                [](const QList<clsORMField>&){ return "A signed JsonWebToken string"; }
    );

    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_String,
                QHttp::RemoteIP_t,
                [](const QHttp::RemoteIP_t& _value) -> QVariant {return _value;},
                [](const QVariant& _value, const QByteArray&) -> QHttp::RemoteIP_t {QHttp::RemoteIP_t Value;Value=_value.toString();return  Value;}
    );


    QHTTP_REGISTER_METATYPE(
                COMPLEXITY_String,
                QHttp::ExtraPath_t,
                [](const QHttp::ExtraPath_t& _value) -> QVariant {return _value;},
                [](const QVariant& _value, const QByteArray&) -> QHttp::ExtraPath_t {
                    QHttp::ExtraPath_t Value;
                    QUrl URL = QUrl::fromPercentEncoding(("http://127.0.0.1/" + _value.toString()).toUtf8());
                    Value=URL.path().remove(0,1);

                    return  Value;
                }
    );

    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::MD5_t, optional(QFV.md5()), _value, [](const QList<clsORMField>&){ return "A valid MD5 string"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::Email_t, optional(QFV.email()), _value, [](const QList<clsORMField>&){ return "A valid email"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::Mobile_t, optional(QFV.mobile()), _value, [](const QList<clsORMField>&){ return "A valid mobile number with/without country code"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::IPv4_t, optional(QFV.ipv4()), _value, [](const QList<clsORMField>&){ return "A valid IP version 4"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::ISO639_2_t, optional(QFV.maxLenght(2).languageCode()), _value, [](const QList<clsORMField>&){ return "A valid ISO639 two-letter language code"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::Base64Image_t, optional(QFV.base64Image()), _value, [](const QList<clsORMField>&){ return "A valid base64 encoded png/jpg image"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::Date_t, optional(QFV.date()), _value, [](const QList<clsORMField>&){ return "A valid gregorian date"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::Time_t, optional(QFV.time()), _value, [](const QList<clsORMField>&){ return "A valid time"; });
    QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(COMPLEXITY_String, QHttp::DateTime_t, optional(QFV.dateTime()), _value, [](const QList<clsORMField>&){ return "A valid datetime"; });
}

stuFileInfo stuFileInfo::fromVariant(const QVariant& _value, const QByteArray& _paramName)
{
     QVariantMap Value = _value.toMap();
     if(Value.isEmpty() || !Value.contains("name") || !Value.contains("tmpname") || !Value.contains("size") || !Value.contains("mime"))
         throw exHTTPBadRequest(_paramName + " is not a valid File information");

     return stuFileInfo(
                 Value["name"].toString(),
                 Value["tmpname"].toString(),
                 Value["size"].toULongLong(),
                 Value["mime"].toString()
                 );
}

QVariant stuFileInfo::toVariant() const{
    return QVariantMap({
                      {"name", this->Name},
                      {"tmpname", this->TempName},
                      {"size", this->Size},
                      {"mime", this->Mime},
                       });
}

QVariant Files_t::toVariant() const
{
    QVariantList Files;
    foreach(auto Val, *this)
        Files.append(Val.toVariant());
    return Files;
}

Files_t Files_t::fromVariant(const QVariant& _value, const QByteArray& _paramName) const
{
    Files_t Files;
    foreach(auto ListItem, _value.toList())
        Files.append(QHttp::stuFileInfo::fromVariant(ListItem, _paramName));
    return Files;
}

}
