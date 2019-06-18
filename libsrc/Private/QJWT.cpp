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
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 */

#include <QStringList>
#include "QJWT.h"
#include "Configs.hpp"

namespace QHttp {
namespace Private{

QByteArray QJWT::createSigned(QJsonObject _payload, const qint32 _expiry, const QString& _sessionID)
{
    const QString Header = QString("{\"typ\":\"JWT\",\"alg\":\"%1\"}").arg(enuJWTHashAlgs::toStr(gConfigs.Public.JWTHashAlgorithm));

    _payload["iat"] = (qint64)QDateTime::currentDateTime().toTime_t();
    if(_expiry >= 0)
        _payload["exp"] = _payload["iat"].toInt() + _expiry;
    else
        _payload.remove("exp");

    if(_sessionID.size())
        _payload["jti"] = _sessionID;
    else
        _payload.remove("jti");

    QByteArray Data   = Header.toUtf8().toBase64() + "." + QJsonDocument(_payload).toJson().toBase64();

    return Data + "." + QJWT::hash(Data).toBase64();
}

QJsonObject QJWT::verifyReturnPayload(const QString &_jwt)
{
    QStringList JWTParts = _jwt.split('.');
    if(JWTParts.length() != 3)
        throw exHTTPForbidden("Invalid JWT Token");
    if(QJWT::hash((JWTParts.at(0) + "." + JWTParts.at(1)).toUtf8()).toBase64() != JWTParts[2])
        throw exHTTPForbidden("JWT signature verification failed");
    QJsonParseError Error;
    QJsonDocument Payload = QJsonDocument::fromJson(JWTParts.at(1).toUtf8(), &Error);
    if(Payload.isNull())
        throw exHTTPForbidden("Invalid JWT payload: " + Error.errorString());

    QJsonObject JWTPayload = Payload.object();
    if(JWTPayload.empty())
        throw exHTTPForbidden("Invalid JWT payload: empty object");
    if(JWTPayload.contains("exp") &&
       (quint64)JWTPayload.value("exp").toInt() <= QDateTime::currentDateTime().toTime_t())
            throw exHTTPUnauthorized("JWT expired");

    return JWTPayload;
}

const QByteArray QJWT::hash(const QByteArray &_data)
{
    switch(gConfigs.Public.JWTHashAlgorithm){
    case enuJWTHashAlgs::HS256:
        return QMessageAuthenticationCode::hash(_data, gConfigs.Public.JWTSecret.toUtf8(), QCryptographicHash::Sha256);
    case enuJWTHashAlgs::HS384:
        return QMessageAuthenticationCode::hash(_data, gConfigs.Public.JWTSecret.toUtf8(), QCryptographicHash::Sha384);
    case enuJWTHashAlgs::HS512:
        return QMessageAuthenticationCode::hash(_data, gConfigs.Public.JWTSecret.toUtf8(), QCryptographicHash::Sha512);
    default:
        throw exHTTPInternalServerError("Invalid JWT encryption algorithm");
    }
}

}
}
