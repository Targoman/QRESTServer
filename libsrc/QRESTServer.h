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

#ifndef QHTTP_QRESTSERVER_H
#define QHTTP_QRESTSERVER_H

#include <QObject>
#include <QScopedPointer>
#include <QHostAddress>
#include "libTargomanCommon/Macros.h"
#include "QHttp/intfRESTAPIHolder.h"

namespace QHttp {

/********************************************************************************/
/**
 * @brief The enuIPBlackListStatus enum is defined to be used as result of fnIsInBlackList_t
 */
TARGOMAN_DEFINE_ENHANCED_ENUM(enuIPBlackListStatus,
                              Ok,
                              Banned,
                              Restricted)

/**
 * @brief The fnIsInBlackList_t type is used to define a IP checker lambda
 */
using fnIsInBlackList_t = std::function<enuIPBlackListStatus::Type (QHostAddress)>;
/********************************************************************************/
/**
 * @brief The RESTServer class is a static class which starts the REST server and listens on a address/port specified in configs.
 *        In order to use this class you must start a QCoreApplication and then call configure"()" if wish to configure server
 *        and then call start"()"
 *
 * @note  This class will create a small thread in oprder to update statistics.
 */
class RESTServer : public QObject
{    
public:
    /**
     * @brief The stuConfig struct contains all the configs of the server.
     */
    struct stuConfig{
        QString BasePath;
        QString Version;
        fnIsInBlackList_t fnIPInBlackList;
        quint8 StatisticsInterval;
        quint16 ListenPort;
        QHostAddress ListenAddress;
        bool         IndentedJson;
        qint64       MaxUploadSize;
        qint64       MaxUploadedFileSize;
        quint32      MaxCachedItems;
        QString      CacheConnector;
#ifdef QHTTP_ENABLE_WEBSOCKET
        QString WebSocketServerName;
        quint16 WebSocketServerPort;
        QString WebSocketServerAdderss;
        bool    WebSocketSecure;
#endif

        stuConfig(const QString& _basePath = "/",
                  const QString& _version = "v0",
                  quint16 _listenPort = 9000,
                  bool _indentedJson = false,
                  const QHostAddress& _listenAddress = QHostAddress::Any,
          #ifdef QHTTP_ENABLE_WEBSOCKET
                  const QString& _websocketServerName = "",
                  quint16        _websocketServerPort = 9010,
                  const QHostAddress& _websocketListenAddress = QHostAddress::Any,
                  bool    _webSocketSecure = false,
          #endif
                  const fnIsInBlackList_t& _ipBlackListChecker = {},
                  const QString& _cacheConnector = "redis://127.0.0.1",
                  quint8 _statisticsInterval = 3,
                  qint64 _maxUploadSize = 100 * 1024 * 1024,
                  qint64 _maxUploadedFileSize = 100 * 1024 * 1024,
                  quint32 _maxCachedItems = 10000
                  );

        stuConfig(const stuConfig& _other);
    };

public:
    /**
     * @brief configure this methods will change server configuration. This method can be called before calling start"()".
     *        After calling start it will throw. To reconfigure server you must first call stop"()"
     * @param _configs configured struct
     */
    static void configure(const stuConfig& _configs);

    /**
     * @brief start will start REST server based on prior configurations
     */
    static void start();

    /**
     * @brief stop will stop server and allow reconfiguration
     */
    static void stop();

    /**
     * @brief shutdown will shutdown server and remove any timer and allocated resources
     */
    static void shutdown();

    /**
     * @brief stats return statistics of the server
     * @return
     */
    static QHttp::stuStatistics stats();

    /**
     * @brief registeredAPIs will return a list of all auto-registered API calls
     * @param _showParams if set to `true` will list API parameters else just API name will be output
     * @param _showTypes if set to `true` will show input types of API parameters and return type else just names will be shown
     * @param _prettifyTypes if set to true a pretty and general form of Qt Types will be printed else will print original QMetaType names
     * @return a list of all APIs registered in the server
     */
    static QStringList registeredAPIs(bool _showParams = true, bool _showTypes = true, bool _prettifyTypes = true);
};

}

#endif // QHTTP_QRESTSERVER_H
