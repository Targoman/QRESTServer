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

#ifndef TARGOMAN_FASTRESTSERVER_PRIVATE_CLSREQUESTHANDLER_H
#define TARGOMAN_FASTRESTSERVER_PRIVATE_CLSREQUESTHANDLER_H

#include "QHttp/QHttpServer"
#include "RESTAPIRegistry.hpp"

namespace Targoman {
namespace FastRESTServer {
namespace Private {

class clsRequestHandler : public QObject
{
public:
    explicit clsRequestHandler(qhttp::server::QHttpRequest* _req, qhttp::server::QHttpResponse* _res);
    void findAndCallAPI(qhttp::server::QHttpRequest *_req, qhttp::server::QHttpResponse *_res);

private:
    void sendError(qhttp::server::QHttpResponse *_res, qhttp::TStatusCode _code, const QString& _message, bool _isUTF8 = false, bool _closeConnection = false);

private:
    RESTAPIRegistry& RESTAPIRegistryInstance;
};

}
}
}
#endif // TARGOMAN_FASTRESTSERVER_PRIVATE_CLSREQUESTHANDLER_H


