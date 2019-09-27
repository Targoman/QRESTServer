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
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */

#ifndef QHTTP_PRIVATE_CLSREQUESTHANDLER_H
#define QHTTP_PRIVATE_CLSREQUESTHANDLER_H

#include <QTemporaryFile>
#include "QHttp/QHttpServer"
#include "RESTAPIRegistry.h"
#include "Private/Configs.hpp"
#include "3rdParty/multipart-parser/MultipartReader.h"

namespace QHttp {
namespace Private {

class clsRequestHandler;

class clsUpdateAndPruneThread : public QThread{
    Q_OBJECT
private:
    void run() Q_DECL_FINAL {
        QTimer Timer;
        QObject::connect(&Timer, &QTimer::timeout, [](){
            gServerStats.Connections.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.WSConnections.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.Errors.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.Blocked.snapshot(gConfigs.Public.StatisticsInterval);
            gServerStats.Success.snapshot(gConfigs.Public.StatisticsInterval);

            for (auto ListIter = gServerStats.APICallsStats.begin ();
                 ListIter != gServerStats.APICallsStats.end ();
                 ++ListIter)
                ListIter->snapshot(gConfigs.Public.StatisticsInterval);
            for (auto ListIter = gServerStats.APIInternalCacheStats.begin ();
                 ListIter != gServerStats.APIInternalCacheStats.end ();
                 ++ListIter)
                ListIter->snapshot(gConfigs.Public.StatisticsInterval);

            QList<Cache_t::const_iterator> ToDeleteIters;
            for(auto CacheIter = InternalCache::Cache.begin();
                CacheIter != InternalCache::Cache.end();
                ++CacheIter)
                if(CacheIter->InsertionTime.secsTo(QTime::currentTime()) > CacheIter->TTL)
                    ToDeleteIters.append(CacheIter);

            if(ToDeleteIters.size()){
                QMutexLocker Locker(&InternalCache::Lock);
                foreach(auto Iter, ToDeleteIters)
                    InternalCache::Cache.erase(Iter);
            }
        });

        Timer.start(gConfigs.Public.StatisticsInterval * 1000);
        this->exec();
    }
};

class clsMultipartFormDataRequestHandler : public MultipartReader{
public:
    clsMultipartFormDataRequestHandler(clsRequestHandler* _parent, const QByteArray& _marker) :
        MultipartReader(_marker.toStdString()),
        pRequestHandler(_parent),
        LastWrittenBytes(0){
        this->onPartBegin = clsMultipartFormDataRequestHandler::onMultiPartBegin;
        this->onPartData = clsMultipartFormDataRequestHandler::onMultiPartData;
        this->onPartEnd = clsMultipartFormDataRequestHandler::onMultiPartEnd;
        this->onEnd = clsMultipartFormDataRequestHandler::onDataEnd;
        this->userData = reinterpret_cast<void*>(this);
    }

    size_t feed(const char *_buffer, size_t _len){
        return MultipartReader::feed(_buffer, _len);
    }

private:
    static void onMultiPartBegin(const MultipartHeaders& _headers, void *_userData);
    static void onMultiPartData(const char *_buffer, long long _size, void *_userData);
    static void onMultiPartEnd(void *_userData);
    static void onDataEnd(void *_userData);

    void storeDataInRequest();

private:
    clsRequestHandler*              pRequestHandler;
    QScopedPointer<QTemporaryFile>  LastTempFile;
    std::string                     LastMime;
    std::string                     LastFileName;
    std::string                     ToBeStoredItemName;
    std::string                     LastItemName;
    QString                         LastValue;
    int                             LastWrittenBytes;
    QStringList                     SameNameItems;

    friend class clsRequestHandler;
};

class clsRequestHandler :QObject
{
public:
    clsRequestHandler(qhttp::server::QHttpRequest* _req,
                      qhttp::server::QHttpResponse* _res,
                      QObject *_parent = nullptr);
    void process(const QString& _api);
    void findAndCallAPI(const QString& _api);
    void sendError(qhttp::TStatusCode _code,
                   const QString& _message,
                   bool _closeConnection = false);
    void sendResponse(qhttp::TStatusCode _code, QVariant _response);
private:
    void sendResponseBase(qhttp::TStatusCode _code, QJsonObject _dataObject, bool _closeConnection = false);
    QString toIPv4(const QString _ip);

private:
    QByteArray                                          RemainingData;
    qhttp::server::QHttpRequest*                        Request;
    qhttp::server::QHttpResponse*                       Response;
    QScopedPointer<clsMultipartFormDataRequestHandler>  MultipartFormDataHandler;

    friend class clsMultipartFormDataRequestHandler;
};

}
}

#endif // QHTTP_PRIVATE_CLSREQUESTHANDLER_H


