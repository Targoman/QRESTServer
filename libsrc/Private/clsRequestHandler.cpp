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

#include "clsRequestHandler.h"
#include "libTargomanCommon/CmdIO.h"
#include "intfRESTAPIHolder.h"

namespace Targoman {
namespace FastRESTServer {
namespace Private {

using namespace qhttp::server;
using namespace Targoman::Common;

clsRequestHandler::clsRequestHandler(qhttp::server::QHttpRequest *_req, qhttp::server::QHttpResponse *_res) :
    QObject(_req),
    RESTAPIRegistryInstance(RESTAPIRegistry::instance())
{
    _req->onData([this, _req, _res](QByteArray _data){
        Q_UNUSED(_data)
    });
    _req->onEnd([this, _req, _res](){
        this->findAndCallAPI (_req, _res);
    });
}

void clsRequestHandler::findAndCallAPI(QHttpRequest *_req, QHttpResponse *_res)
{
    clsAPIObject* APIObject = this->RESTAPIRegistryInstance.getAPIObject(_req->methodString(), _req->url().path());

    if(!APIObject)
        return this->sendError(_res,
                               qhttp::ESTATUS_BAD_REQUEST,
                               "Invalid API call ("+_req->methodString()+": "+_req->url().path()+")",
                               true);

    try{
        //APIObject->invoke();
    }catch(exHTTPError &ex){
        this->sendError(_res, (qhttp::TStatusCode)ex.code(), ex.what(), true, ex.code() >= 500);
    }
}

void clsRequestHandler::sendError(QHttpResponse *_res, qhttp::TStatusCode _code, const QString &_message, bool _isUTF8, bool _closeConnection)
{
    _res->setStatusCode(_code);
    if(_closeConnection) _res->addHeader("connection", "close");
    _res->addHeaderValue("content-length", _message.length());
    _res->end(Q_UNLIKELY(_isUTF8) ? _message.toUtf8().constData() : _message.toLatin1().constData());
}

}
}
}
