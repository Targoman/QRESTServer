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
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#ifndef QHTTP_PRIVATE_CLSREQUESTHANDLER_H
#define QHTTP_PRIVATE_CLSREQUESTHANDLER_H

#include "QHttp/QHttpServer"
#include "RESTAPIRegistry.h"
#include "Private/Configs.hpp"

namespace QHttp {
namespace Private {

class clsStatisticsUpdateThread : public QThread{
    Q_OBJECT
private:
    void run() Q_DECL_FINAL {
        QTimer Timer;
        QObject::connect(&Timer, &QTimer::timeout, [](){
            gServerStats.Connections.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.Errors.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.Success.snapshot(gConfigs.Public.StatisticsInterval);

            for (auto ListIter = gServerStats.APICallsStats.begin ();
                 ListIter != gServerStats.APICallsStats.end ();
                 ++ListIter)
                ListIter->snapshot(gConfigs.Public.StatisticsInterval);
            for (auto ListIter = gServerStats.APICacheStats.begin ();
                 ListIter != gServerStats.APICacheStats.end ();
                 ++ListIter)
                ListIter->snapshot(gConfigs.Public.StatisticsInterval);
        });
        Timer.start(gConfigs.Public.StatisticsInterval * 1000);
        this->exec();
    }
};

class clsRequestHandler
{
public:
    void process(const QString &_api, qhttp::server::QHttpRequest* _req, qhttp::server::QHttpResponse* _res);
    void findAndCallAPI(const QString &_api, qhttp::server::QHttpRequest *_req, qhttp::server::QHttpResponse *_res);
    void sendError(qhttp::server::QHttpResponse *_res, qhttp::TStatusCode _code, const QString& _message, bool _closeConnection = false);
    void sendResponse(qhttp::server::QHttpResponse *_res, qhttp::TStatusCode _code, QVariant _response);

private:
    QByteArray RemainingData;
};

}
}

#endif // QHTTP_PRIVATE_CLSREQUESTHANDLER_H


