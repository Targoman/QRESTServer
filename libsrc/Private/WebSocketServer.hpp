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

namespace QHttp {
namespace Private {

class WebSocketServer : public QObject{
    Q_OBJECT
public:
    void start(){
        this->WS.reset = new QWebSocketServer(gConfigs.Public.WebSocketServerName,
                                              gConfigs.Public.WebSocketSecure ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode);


        if (this->WS->listen(QHostAddress::Any, gConfigs.Public.WebSocketServerPort)) {
            TargomanInfo(1, "WebSocketServer is listening on "<<gConfigs.Public.WebSocketServerPort<< " Name: "<<gConfigs.Public.WebSocketServerName);
            connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this,SLOT(onNewConnection());
            connect(gWSServer, &QWebSocketServer::closed, this, &WebSocketServer::closed);
        }
    }

    void stopListening(){
        this->WS->close ();
        qDeleteAll(this->Clients.begin(), this->Clients.end());
    }
signals:
    void sigNewConnection(QWebSocket* _connection);

private slots:
    void onNewConnection(){
        QWebSocket *pSocket = this->WS->nextPendingConnection();

        connect(pSocket, &QWebSocket::textMessageReceived, this, SLOT(processTextMessage(QString)));
        connect(pSocket, &QWebSocket::binaryMessageReceived, this, SLOT(processBinaryMessage(QByteArray)));
        connect(pSocket, &QWebSocket::disconnected, this, SLOT(socketDisconnected()));

        this->Clients << pSocket;

        emit sigNewConnection(pSocket);
    }
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected(){
        QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
        if (pClient) {
            TargomanLogDebug(5, "Client Disconnected: "<<pClient)
            this->Clients.removeAll(pClient);
            this->Clients->deleteLater();
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
