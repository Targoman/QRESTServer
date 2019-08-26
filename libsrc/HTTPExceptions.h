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

#ifndef QHTTP_HTTPEXCEPTIONS_H
#define QHTTP_HTTPEXCEPTIONS_H

#include "libTargomanCommon/exTargomanBase.h"

namespace QHttp {
/*******************************************************************************/
class exHTTPError : public Targoman::Common::exTargomanBase {
public:
    exHTTPError(const QString& _definition, quint16 _errorCode, const QString& _message, int _line):
        Targoman::Common::exTargomanBase(_message, _line),
        Definition(_definition),
        Code(_errorCode)
    {}

    virtual void toEnsureAvoidanceOfUsingBaseClass()=0;

    inline quint16 code() const{return this->Code;}
    inline const QString definition() const{return this->Definition;}
    inline const QString fullError(){
        return QString("%1(%2): %3").arg(
                    this->definition()).arg(
                    this->code()).arg(
                    this->what());}

protected:
  QString    Definition;
  quint16     Code;
};

/*******************************************************************************/
#define QRESTSERVER_ADD_HTTP_EXCEPTION(_code,_name) \
    class _name : public exHTTPError{\
    public: _name (const QString& _message = "", int _line = 0) : \
            exHTTPError (QString(TARGOMAN_M2STR(_name)).mid(2), _code, _message, _line) \
            {} \
            void toEnsureAvoidanceOfUsingBaseClass(){} \
    }

/*******************************************************************************/
#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wweak-vtables"
QRESTSERVER_ADD_HTTP_EXCEPTION(400, exHTTPBadRequest);
QRESTSERVER_ADD_HTTP_EXCEPTION(401, exHTTPUnauthorized);
QRESTSERVER_ADD_HTTP_EXCEPTION(402, exHTTPPaymentRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(403, exHTTPForbidden);
QRESTSERVER_ADD_HTTP_EXCEPTION(404, exHTTPNotFound);
QRESTSERVER_ADD_HTTP_EXCEPTION(405, exHTTPMethodNotAllowed);
QRESTSERVER_ADD_HTTP_EXCEPTION(406, exHTTPNotAcceptable);
QRESTSERVER_ADD_HTTP_EXCEPTION(407, exHTTPProxyAuthenticationRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(408, exHTTPRequestTimeout);
QRESTSERVER_ADD_HTTP_EXCEPTION(409, exHTTPConflict);
QRESTSERVER_ADD_HTTP_EXCEPTION(410, exHTTPGone);
QRESTSERVER_ADD_HTTP_EXCEPTION(411, exHTTPLengthRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(412, exHTTPPreconditionFailed);
QRESTSERVER_ADD_HTTP_EXCEPTION(413, exHTTPPayloadTooLarge);
QRESTSERVER_ADD_HTTP_EXCEPTION(414, exHTTPURITooLong);
QRESTSERVER_ADD_HTTP_EXCEPTION(415, exHTTPUnsupportedMediaType);
QRESTSERVER_ADD_HTTP_EXCEPTION(416, exHTTPRangeNotSatisfiable);
QRESTSERVER_ADD_HTTP_EXCEPTION(417, exHTTPExpectationFailed);
QRESTSERVER_ADD_HTTP_EXCEPTION(421, exHTTPMisdirectedRequest);
QRESTSERVER_ADD_HTTP_EXCEPTION(422, exHTTPUnprocessableEntity);
QRESTSERVER_ADD_HTTP_EXCEPTION(423, exHTTPLocked);
QRESTSERVER_ADD_HTTP_EXCEPTION(424, exHTTPFailedDependency);
QRESTSERVER_ADD_HTTP_EXCEPTION(426, exHTTPUpgradeRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(428, exHTTPPreconditionRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(429, exHTTPTooManyRequests);
QRESTSERVER_ADD_HTTP_EXCEPTION(431, exHTTPRequestHeaderFieldsTooLarge);
QRESTSERVER_ADD_HTTP_EXCEPTION(431, exHTTPLoginTimedout);
QRESTSERVER_ADD_HTTP_EXCEPTION(451, exHTTPUnavailableForLegalReasons);
QRESTSERVER_ADD_HTTP_EXCEPTION(498, exHTTPInvalidToken);
QRESTSERVER_ADD_HTTP_EXCEPTION(499, exHTTPTokenRequired);
//-----------------------------------------------------------------
QRESTSERVER_ADD_HTTP_EXCEPTION(500, exHTTPInternalServerError);
QRESTSERVER_ADD_HTTP_EXCEPTION(501, exHTTPNotImplemented);
QRESTSERVER_ADD_HTTP_EXCEPTION(502, exHTTPBadGateway);
QRESTSERVER_ADD_HTTP_EXCEPTION(503, exHTTPServiceUnavailable);
QRESTSERVER_ADD_HTTP_EXCEPTION(504, exHTTPGatewayTimeout);
QRESTSERVER_ADD_HTTP_EXCEPTION(505, exHTTPVersionNotSupported);
QRESTSERVER_ADD_HTTP_EXCEPTION(506, exHTTPVariantAlsoNegotiates);
QRESTSERVER_ADD_HTTP_EXCEPTION(507, exHTTPInsufficientStorage);
QRESTSERVER_ADD_HTTP_EXCEPTION(508, exHTTPLoopDetected);
QRESTSERVER_ADD_HTTP_EXCEPTION(510, exHTTPNotExtended);
QRESTSERVER_ADD_HTTP_EXCEPTION(511, exHTTPNetworkAuthenticationRequired);
QRESTSERVER_ADD_HTTP_EXCEPTION(520, exHTTPUnknownError);
#pragma GCC diagnostic pop
/*******************************************************************************/

}
#endif // QHTTP_HTTPEXCEPTIONS_H
