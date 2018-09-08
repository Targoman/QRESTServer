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

    virtual void toEnsureAvoidanceOFUsingBaseClass()=0;

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
#define TARGOMAN_ADD_HTTP_EXCEPTION(_code,_name) \
    class _name : public exHTTPError{\
    public: _name (const QString& _message = "", int _line = 0) : \
            exHTTPError (QString(TARGOMAN_M2STR(_name)).mid(2), _code, _message, _line) \
            {} \
            void toEnsureAvoidanceOFUsingBaseClass(){} \
    }

/*******************************************************************************/
TARGOMAN_ADD_HTTP_EXCEPTION(400, exHTTPBadRequest);
TARGOMAN_ADD_HTTP_EXCEPTION(401, exHTTPUnauthorized);
TARGOMAN_ADD_HTTP_EXCEPTION(402, exHTTPPaymentRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(403, exHTTPForbidden);
TARGOMAN_ADD_HTTP_EXCEPTION(404, exHTTPNotFound);
TARGOMAN_ADD_HTTP_EXCEPTION(405, exHTTPMethodNotAllowed);
TARGOMAN_ADD_HTTP_EXCEPTION(406, exHTTPNotAcceptable);
TARGOMAN_ADD_HTTP_EXCEPTION(407, exHTTPProxyAuthenticationRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(408, exHTTPRequestTimeout);
TARGOMAN_ADD_HTTP_EXCEPTION(409, exHTTPConflict);
TARGOMAN_ADD_HTTP_EXCEPTION(410, exHTTPGone);
TARGOMAN_ADD_HTTP_EXCEPTION(411, exHTTPLengthRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(412, exHTTPPreconditionFailed);
TARGOMAN_ADD_HTTP_EXCEPTION(413, exHTTPPayloadTooLarge);
TARGOMAN_ADD_HTTP_EXCEPTION(414, exHTTPURITooLong);
TARGOMAN_ADD_HTTP_EXCEPTION(415, exHTTPUnsupportedMediaType);
TARGOMAN_ADD_HTTP_EXCEPTION(416, exHTTPRangeNotSatisfiable);
TARGOMAN_ADD_HTTP_EXCEPTION(417, exHTTPExpectationFailed);
TARGOMAN_ADD_HTTP_EXCEPTION(421, exHTTPMisdirectedRequest);
TARGOMAN_ADD_HTTP_EXCEPTION(422, exHTTPUnprocessableEntity);
TARGOMAN_ADD_HTTP_EXCEPTION(423, exHTTPLocked);
TARGOMAN_ADD_HTTP_EXCEPTION(424, exHTTPFailedDependency);
TARGOMAN_ADD_HTTP_EXCEPTION(426, exHTTPUpgradeRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(428, exHTTPPreconditionRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(429, exHTTPTooManyRequests);
TARGOMAN_ADD_HTTP_EXCEPTION(431, exHTTPRequestHeaderFieldsTooLarge);
TARGOMAN_ADD_HTTP_EXCEPTION(431, exHTTPLoginTimedout);
TARGOMAN_ADD_HTTP_EXCEPTION(451, exHTTPUnavailableForLegalReasons);
TARGOMAN_ADD_HTTP_EXCEPTION(498, exHTTPInvalidToken);
TARGOMAN_ADD_HTTP_EXCEPTION(499, exHTTPTokenRequired);
//-----------------------------------------------------------------
TARGOMAN_ADD_HTTP_EXCEPTION(500, exHTTPInternalServerError);
TARGOMAN_ADD_HTTP_EXCEPTION(501, exHTTPNotImplemented);
TARGOMAN_ADD_HTTP_EXCEPTION(502, exHTTPBadGateway);
TARGOMAN_ADD_HTTP_EXCEPTION(503, exHTTPServiceUnavailable);
TARGOMAN_ADD_HTTP_EXCEPTION(504, exHTTPGatewayTimeout);
TARGOMAN_ADD_HTTP_EXCEPTION(505, exHTTPVersionNotSupported);
TARGOMAN_ADD_HTTP_EXCEPTION(506, exHTTPVariantAlsoNegotiates);
TARGOMAN_ADD_HTTP_EXCEPTION(507, exHTTPInsufficientStorage);
TARGOMAN_ADD_HTTP_EXCEPTION(508, exHTTPLoopDetected);
TARGOMAN_ADD_HTTP_EXCEPTION(510, exHTTPNotExtended);
TARGOMAN_ADD_HTTP_EXCEPTION(511, exHTTPNetworkAuthenticationRequired);
TARGOMAN_ADD_HTTP_EXCEPTION(520, exHTTPUnknownError);
/*******************************************************************************/

}
#endif // QHTTP_HTTPEXCEPTIONS_H
