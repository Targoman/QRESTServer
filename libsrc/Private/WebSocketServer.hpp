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

#ifndef QHTTP_WEBSOCKETSERVER_HPP
#define QHTTP_WEBSOCKETSERVER_HPP

#ifdef QHTTP_ENABLE_WEBSOCKET
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

#include "libTargomanCommon/Logger.h"
#include "Private/Configs.hpp"
#include "Private/RESTAPIRegistry.h"

namespace QHttp {
namespace Private {

class WebSocketServer : public QObject{
    Q_OBJECT

public:
    void start(){
        this->WS.reset(new QWebSocketServer(gConfigs.Public.WebSocketServerName,
                                            gConfigs.Public.WebSocketSecure ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode));


        if (this->WS->listen(QHostAddress::Any, gConfigs.Public.WebSocketServerPort)) {
            TargomanLogInfo(1, "WebSocketServer is listening on "<<gConfigs.Public.WebSocketServerPort<< " Name: "<<gConfigs.Public.WebSocketServerName);
            connect(this->WS.data(), &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
            connect(this->WS.data(), &QWebSocketServer::closed, this, &WebSocketServer::closed);
        }
    }

    void stopListening(){
        this->WS->close ();
        qDeleteAll(this->Clients.begin(), this->Clients.end());
    }
signals:
    void sigNewConnection(QWebSocket* _connection);
    void closed();

private slots:
    void onNewConnection(){
        QWebSocket *pSocket = this->WS->nextPendingConnection();

        connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::processTextMessage);
        connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketServer::processBinaryMessage);
        connect(pSocket, &QWebSocket::disconnected, this, &WebSocketServer::socketDisconnected);

        this->Clients << pSocket;

        emit sigNewConnection(pSocket);
    }

    void processTextMessage(QString _message){
        QWebSocket *pSocket = qobject_cast<QWebSocket *>(sender());
        TargomanDebug(5, "Text Message Received:" << _message);

        if (pSocket) {
            auto sendError = [pSocket](qhttp::TStatusCode _code, QString _message){
                QJsonObject ErrorInfo = QJsonObject({
                                                        {"code", _code},
                                                        {"message", _message}
                                                    });
                pSocket->sendTextMessage(QJsonDocument(QJsonObject({{"error",
                                                                     ErrorInfo
                                                                    }})).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact).data());
            };

            try{
                QJsonParseError Error;
                QJsonDocument JSON = QJsonDocument::fromJson(_message.toUtf8(), &Error);
                if(JSON.isNull())
                    throw exHTTPBadRequest(QString("Invalid JSON request: %1").arg(Error.errorString()));

                QJsonObject JSONReqObject = JSON.object();
                clsAPIObject* APIObject = RESTAPIRegistry::getWSAPIObject (JSONReqObject.begin().key());
                QString API = JSONReqObject.begin().key();
                QVariantMap APIArgs = JSONReqObject.begin().value().toObject().toVariantMap();
                QStringList Queries;
                for(auto Map = APIArgs.begin(); Map != APIArgs.end(); ++Map)
                    Queries.append(Map.key() + '=' + Map.value().toString());


                if(!APIObject)
                    return sendError(qhttp::ESTATUS_NOT_FOUND, "WS API not found ("+API+")");
                QByteArray Data = QJsonDocument(QJsonObject({{"result",
                                                              QJsonValue::fromVariant(APIObject->invoke(Queries))
                                                             }})).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);
                pSocket->sendTextMessage(Data.data());
            }catch(exHTTPError &ex){
                sendError((qhttp::TStatusCode)ex.code(), ex.what());
            }catch(Targoman::Common::exTargomanBase &ex){
                sendError(qhttp::ESTATUS_INTERNAL_SERVER_ERROR, ex.what());
            }
        }
    }

    void processBinaryMessage(QByteArray _message){
        QWebSocket *pSocket = qobject_cast<QWebSocket *>(sender());
        TargomanDebug(5, "Binary Message Received:" << _message);
        if (pSocket) {
            pSocket->sendBinaryMessage(_message);
        }
    }

    void socketDisconnected(){
        QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
        if (pClient) {
            TargomanLogDebug(5, "Client Disconnected: "<<pClient)
                    this->Clients.removeAll(pClient);
            pClient->deleteLater();
        }
    }

private:
    QScopedPointer<QWebSocketServer> WS;
    QList<QWebSocket *> Clients;
};

}
}
#endif

#endif // QHTTP_CLSWEBSOCKETSERVER_HPP
