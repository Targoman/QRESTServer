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
#include "Private/clsRedisConnector.h"
#include "Private/WebSocketServer.hpp"
#include "Private/RESTAPIRegistry.h"
#include "Private/QJWT.h"
#include "QHttp/qhttpfwd.hpp"

namespace QHttp {

using namespace Targoman::Common;
using namespace qhttp::server;
using namespace Private;

bool validateConnection(const QHostAddress& _peerAddress, quint16 _peerPort){
    enuIPBlackListStatus::Type IPBlackListStatus = enuIPBlackListStatus::Unknown;

    if(gConfigs.Public.fnIPInBlackList &&
            (IPBlackListStatus = gConfigs.Public.fnIPInBlackList(_peerAddress)) != enuIPBlackListStatus::Ok){
        TargomanLogWarn(1,"Connection from " + _peerAddress.toString() + " was closed by security provider due to: "+enuIPBlackListStatus::toStr(IPBlackListStatus));
        gServerStats.Blocked.inc();
        return false;
    }
    gServerStats.Connections.inc();

    TargomanLogInfo(7, "New connection accepted from: "<<_peerAddress.toString()<<":"<<_peerPort);
    return true;
}

void RESTServer::configure(const RESTServer::stuConfig& _configs) {
    if(gConfigs.Private.IsStarted)
        throw exTargomanInitialization("QRESTServer can not be reconfigured while listening");

    gConfigs.Public = _configs;
}

static qhttp::server::QHttpServer gHTTPServer;
#ifdef QHTTP_ENABLE_WEBSOCKET
static WebSocketServer gWSServer;
#endif
static clsUpdateAndPruneThread *gStatUpdateThread;

void RESTServer::start() {
    if(gConfigs.Private.IsStarted)
        throw exTargomanInitialization("QRESTServer can be started one time only");

    if(gConfigs.Public.BasePath.endsWith('/') == false)
        gConfigs.Public.BasePath+='/';

    if(gConfigs.Public.BasePath.startsWith('/') == false)
        gConfigs.Public.BasePath='/'+gConfigs.Public.BasePath;

    if(gConfigs.Public.CacheConnector.size() && QUrl::fromUserInput(gConfigs.Public.CacheConnector).isValid() == false)
        throw exRESTRegistry("Invalid connector url specified for central cache");

#ifdef QHTTP_REDIS_PROTOCOL
    if(gConfigs.Public.CacheConnector.startsWith(TARGOMAN_M2STR(QHTTP_REDIS_PROTOCOL)))
        CentralCache::setup(new clsRedisConnector(gConfigs.Public.CacheConnector));
#endif

    if(gConfigs.Public.CacheConnector.size() && CentralCache::isValid() == false)
        throw exRESTRegistry("Unsupported cache connector protocol.");

    gConfigs.Private.BasePathWithVersion = gConfigs.Public.BasePath + gConfigs.Public.Version;
    if(gConfigs.Private.BasePathWithVersion.endsWith('/') == false)
        gConfigs.Private.BasePathWithVersion += '/';

    gConfigs.Private.IsStarted = true;

    if(gConfigs.Public.StatisticsInterval){
        gStatUpdateThread = new clsUpdateAndPruneThread();
        connect(gStatUpdateThread, &clsUpdateAndPruneThread::finished, gStatUpdateThread, &QObject::deleteLater);
        gStatUpdateThread->start();
    }


    QObject::connect(&gHTTPServer, &QHttpServer::newConnection, [](QHttpConnection* _con){
        if(!validateConnection (_con->tcpSocket()->peerAddress(), _con->tcpSocket()->peerPort()))
            _con->killConnection();
    });

    gHTTPServer.listen (gConfigs.Public.ListenAddress, gConfigs.Public.ListenPort, [&](QHttpRequest* _req, QHttpResponse* _res){
        clsRequestHandler* RequestHandler = new clsRequestHandler(_req, _res);
        try{
            QString Path = _req->url().adjusted(QUrl::NormalizePathSegments |
                                                QUrl::RemoveAuthority
                                                ).path(QUrl::PrettyDecoded);

            if(Path.startsWith(gConfigs.Public.BasePath) == false)
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_FOUND, "Path not found: '" + Path + "'", true);
            if(Path.startsWith(gConfigs.Private.BasePathWithVersion) == false)
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_ACCEPTABLE, "Invalid Version or version not specified", true);
            if(Path == gConfigs.Private.BasePathWithVersion )
                return RequestHandler->sendError(qhttp::ESTATUS_NOT_ACCEPTABLE, "No API call provided", true);

            TargomanLogInfo(7,
                            "New API Call ["<<
                            _req->connection()->tcpSocket()->peerAddress().toString()<<
                            ":"<<
                            _req->connection()->tcpSocket()->peerPort()<<
                            "]: "<<
                            Path<<
                            "?"<<
                            _req->url().query());
            RequestHandler->process(Path.mid(gConfigs.Private.BasePathWithVersion.size() - 1));
        }catch(exTargomanBase& ex){
            RequestHandler->sendError(static_cast<qhttp::TStatusCode>(ex.httpCode()), ex.what(), ex.httpCode() >= 500);
        }
    });

    if(gHTTPServer.isListening()){
        TargomanLogInfo(1, "REST Server is listening on "<<gConfigs.Public.ListenAddress.toString()<<":"<<gConfigs.Public.ListenPort);
    }else{
        TargomanLogError("Unable to start server to listen on "<<gConfigs.Public.ListenAddress.toString()<<":"<<gConfigs.Public.ListenPort);
        exit (1);
    }

#ifdef QHTTP_ENABLE_WEBSOCKET
    if(gConfigs.Public.WebSocketServerName.size()){
        QObject::connect(&gWSServer, &WebSocketServer::sigNewConnection, [](QWebSocket* _con){
            if(!validateConnection (_con->peerAddress(), _con->peerPort()))
                _con->close(QWebSocketProtocol::CloseCodePolicyViolated,"IP banned");
        });

        gWSServer.start();
    }
#endif
}

void RESTServer::stop()
{
    gHTTPServer.stopListening();
#ifdef QHTTP_ENABLE_WEBSOCKET
    gWSServer.stopListening();
#endif

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

void RESTServer::registerUserDefinedType(const char* _typeName, intfAPIArgManipulator* _argManipulator)
{
    Q_ASSERT_X(QMetaType::type(_typeName), "registerUserDefinedType", "Seems that registering syntax is erroneous");
    Private::gUserDefinedTypesInfo.insert(QMetaType::type(_typeName) - 1025, _argManipulator);
}

/***********************************************************************************************/
intfRESTAPIHolder::intfRESTAPIHolder(Targoman::Common::Configuration::intfModule *_parent) :
    Targoman::Common::Configuration::intfModule(_parent)
{
    registerGenericTypes();
}

void intfRESTAPIHolder::registerMyRESTAPIs(){
    for (int i=0; i<this->metaObject()->methodCount(); ++i)
        //if(this->metaObject()->method(i).methodType() == QMetaMethod::Slot)
            RESTAPIRegistry::registerRESTAPI(this, this->metaObject()->method(i));
}


QHttp::EncodedJWT_t intfRESTAPIHolder::createSignedJWT(QJsonObject _payload, QJsonObject _privatePayload, const qint32 _expiry, const QString& _sessionID)
{
    return Private::QJWT::createSigned(_payload, _privatePayload, _expiry, _sessionID);
}

QStringList intfRESTAPIHolder::apiGETListOfAPIs(bool _showParams, bool _showTypes, bool _prettifyTypes){
    return RESTAPIRegistry::registeredAPIs("", _showParams, _showTypes, _prettifyTypes);
}

intfAPIArgManipulator::intfAPIArgManipulator(const QString& _realTypeName)
{
    this->PrettyTypeName = (_realTypeName.startsWith('Q') ? _realTypeName.mid(1) : _realTypeName).toLower();
    this->RealTypeName = new char[_realTypeName.toStdString().size()];
    strcpy(this->RealTypeName, _realTypeName.toStdString().c_str());
}

intfAPIArgManipulator::~intfAPIArgManipulator()
{;}

intfAPIObject::~intfAPIObject()
{;}

}


