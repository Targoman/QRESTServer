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

#include <QThread>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>
#include "QRESTServer.h"
#include "Private/Configs.hpp"
#include "Private/clsRequestHandler.h"

namespace QHttp {

using namespace Targoman::Common;
using namespace qhttp::server;
using namespace Private;

void RESTServer::configure(const RESTServer::stuConfig &_configs) {
    if(gConfigs.Private.IsStarted)
        throw exTargomanInitialization("QRESTServer can not be reconfigured while listening");

    gConfigs.Public = _configs;
}

static qhttp::server::QHttpServer gHTTPServer;
static clsUpdateAndPruneThread *gStatUpdateThread;

void RESTServer::start() {
    if(gConfigs.Private.IsStarted)
        throw exTargomanInitialization("QRESTServer can be started one time only");

    gConfigs.Private.BaseParthWithVersion = gConfigs.Public.BasePath + gConfigs.Public.Version;
    if(gConfigs.Private.BaseParthWithVersion.endsWith('/') == false)
        gConfigs.Private.BaseParthWithVersion += '/';

    gConfigs.Private.IsStarted = true;

    if(gConfigs.Public.StatisticsInterval){
        gStatUpdateThread = new clsUpdateAndPruneThread();
        connect(gStatUpdateThread, &clsUpdateAndPruneThread::finished, gStatUpdateThread, &QObject::deleteLater);
        gStatUpdateThread->start();
    }


    QObject::connect(&gHTTPServer, &QHttpServer::newConnection, [](QHttpConnection* _con){
        QTcpSocket* Connection = _con->tcpSocket();

        enuIPBlackListStatus::Type IPBlackListStatus;

        if(gConfigs.Public.fnIPInBlackList &&
                (IPBlackListStatus = gConfigs.Public.fnIPInBlackList(Connection->peerAddress())) != enuIPBlackListStatus::Ok){
            TargomanWarn(1,"Connection from " + Connection->peerAddress().toString() + " was closed by security provider due to: "+enuIPBlackListStatus::toStr(IPBlackListStatus))
                    _con->killConnection();
            gServerStats.Blocked.inc();
            return;
        }
        gServerStats.Connections.inc();

        TargomanLogInfo(7, "New connection: "<<Connection->peerAddress().toString().toLatin1().constData()<<":"<<Connection->peerPort());
    });

    gHTTPServer.listen (gConfigs.Public.ListenAddress, gConfigs.Public.ListenPort, [&](QHttpRequest* _req, QHttpResponse* _res){
        clsRequestHandler* RequestHandler = new clsRequestHandler(_req, _res);
        try{
            QString Path = _req->url().adjusted(QUrl::NormalizePathSegments |
                                                QUrl::RemoveAuthority
                                                ).path(QUrl::PrettyDecoded);

            if(Path.startsWith(gConfigs.Public.BasePath) == false)
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_FOUND, "Path not found: '" + Path + "'", true);
            if(Path.startsWith(gConfigs.Private.BaseParthWithVersion) == false)
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_ACCEPTABLE, "Invalid Version or version not specified", true);
            if(Path == gConfigs.Private.BaseParthWithVersion )
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_ACCEPTABLE, "No API call provided", true);

            TargomanLogInfo(7,
                            "New API Call ["<<
                            _req->connection()->tcpSocket()->peerAddress().toString()<<
                            ":"<<
                            _req->connection()->tcpSocket()->peerPort()<<
                            "]: "<<
                            Path<<
                            _req->url().query());
            RequestHandler->process(Path.mid(gConfigs.Private.BaseParthWithVersion.size() - 1));
        }catch(exHTTPError &ex){
            RequestHandler->sendError((qhttp::TStatusCode)ex.code(), ex.what(), ex.code() >= 500);
        }catch(exTargomanBase &ex){
            RequestHandler->sendError(qhttp::ESTATUS_INTERNAL_SERVER_ERROR, ex.what(), true);
        }
    });

    if(gHTTPServer.isListening()){
        TargomanInfo(1, "Server is listening on "<<gConfigs.Public.ListenAddress.toString()<<":"<<gConfigs.Public.ListenPort);
    }else{
        TargomanError("Unable to start server to listen on "<<gConfigs.Public.ListenAddress.toString()<<":"<<gConfigs.Public.ListenPort);
        exit (1);
    }
}

void RESTServer::stop()
{
    gHTTPServer.stopListening();
    gConfigs.Private.IsStarted = false;
    if(gStatUpdateThread)
        gStatUpdateThread->quit();
}

stuStatistics RESTServer::stats()
{
    return gServerStats;
}

QStringList RESTServer::registeredAPIs(bool _showParams, bool _showTypes, bool _prettifyTypes)
{
    return RESTAPIRegistry::registeredAPIs("", _showParams, _showTypes, _prettifyTypes);
}

/***********************************************************************************************/
intfRESTAPIHolder::intfRESTAPIHolder(Targoman::Common::Configuration::intfModule *_parent) :
    Targoman::Common::Configuration::intfModule(_parent)
{
    QHTTP_REGISTER_METATYPE(
        QHttp::stuTable,
                [](const QHttp::stuTable& _value) -> QVariant{
                    return QVariantMap({{"totalRows", _value.TotalRows}, {"Rows", _value.Rows}});
                },
                nullptr
    );


    //QHTTP_REGISTER_METATYPE(QHttp::stuStatistics);
}

void intfRESTAPIHolder::registerMyRESTAPIs(){
    for (int i=0; i<this->metaObject()->methodCount(); ++i)
        RESTAPIRegistry::registerRESTAPI(this, this->metaObject()->method(i));

}

QStringList intfRESTAPIHolder::apiGETListOfAPIs(bool _showParams, bool _showTypes, bool _prettifyTypes){
    return RESTAPIRegistry::registeredAPIs("", _showParams, _showTypes, _prettifyTypes);
}

intfAPIArgManipulator::intfAPIArgManipulator(const QString &_realTypeName)
{
    this->PrettyTypeName = (_realTypeName.startsWith('Q') ? _realTypeName.mid(1) : _realTypeName).toLower();
    this->RealTypeName = new char[_realTypeName.toStdString().size()];
    strcpy(this->RealTypeName, _realTypeName.toStdString().c_str());
}

/***********************************************************************************************/
QHash<QString, clsAPIObject*>  RESTAPIRegistry::Registry;

RESTServer::stuConfig::stuConfig(const QString &_basePath,
                                 const QString &_version,
                                 quint16 _listenPort,
                                 bool _indentedJson,
                                 const QHostAddress &_listenAddress,
                                 const fnIsInBlackList_t &_ipBlackListChecker,
                                 quint8 _statisticsInterval,
                                 qint64 _maxUploadSize,
                                 qint64 _maxUploadedFileSize,
                                 quint32 _maxCachedItems):
    BasePath(_basePath),
    Version(_version),
    fnIPInBlackList(_ipBlackListChecker),
    StatisticsInterval(_statisticsInterval),
    ListenPort(_listenPort),
    ListenAddress(_listenAddress),
    IndentedJson(_indentedJson),
    MaxUploadSize(_maxUploadSize),
    MaxUploadedFileSize(_maxUploadedFileSize),
    MaxCachedItems(_maxCachedItems)
{
    if(this->BasePath.endsWith('/') == false)
        this->BasePath+='/';
}

}


