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
 * @author S. Mehran M. Ziabary <ziabary@targoman.com>
 */

#ifndef QHTTP_GENERICTYPES_H
#define QHTTP_GENERICTYPES_H

#include <QJsonDocument>
#include <QJsonObject>
#include "libTargomanCommon/clsCountAndSpeed.h"
#include "QHttp/qhttpfwd.hpp"
#include "QHttp/tmplAPIArg.h"

namespace QHttp {
/**
 * @brief The stuStatistics struct holds server statistics about APIs
 */
struct stuStatistics {
    Targoman::Common::clsCountAndSpeed Connections;
    Targoman::Common::clsCountAndSpeed WSConnections;
    Targoman::Common::clsCountAndSpeed Errors;
    Targoman::Common::clsCountAndSpeed Blocked;
    Targoman::Common::clsCountAndSpeed Success;

    QHash<QByteArray, Targoman::Common::clsCountAndSpeed> APICallsStats;
    QHash<QByteArray, Targoman::Common::clsCountAndSpeed> APIInternalCacheStats;
    QHash<QByteArray, Targoman::Common::clsCountAndSpeed> APICentralCacheStats;
};

/**********************************************************************/
/**
 * @brief The stuTable struct
 */
struct stuTable{
    qint64 TotalRows;
    QVariantList Rows;
    stuTable(qint64 _totalRows = -1, const QVariantList& _rows = QVariantList()):
        TotalRows(_totalRows),
        Rows(_rows)
    {}
    QVariant toVariant(){
      return QVariantMap({
                           {"rows", this->Rows},
                           {"totalRows", this->TotalRows}
                         });
    }
};

/**********************************************************************/

QHTTP_ADD_COMPLEX_TYPE(qhttp::THeaderHash, HEADERS_t);
QHTTP_ADD_COMPLEX_TYPE(qhttp::THeaderHash, COOKIES_t);

QHTTP_ADD_SIMPLE_TYPE(QJsonObject, JWT_t);
QHTTP_ADD_SIMPLE_TYPE(QJsonDocument, JSON_t);

QHTTP_ADD_SIMPLE_TYPE(QString, EncodedJWT_t);
QHTTP_ADD_SIMPLE_TYPE(QString, ExtraPath_t);
QHTTP_ADD_SIMPLE_TYPE(QString, RemoteIP_t);
QHTTP_ADD_SIMPLE_TYPE(QString, MD5_t);
QHTTP_ADD_SIMPLE_TYPE(QString, IPv4_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Email_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Mobile_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Sheba_t);
QHTTP_ADD_SIMPLE_TYPE(QString, ISO639_2_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Base64Image_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Date_t);
QHTTP_ADD_SIMPLE_TYPE(QString, Time_t);
QHTTP_ADD_SIMPLE_TYPE(QString, DateTime_t);

/**********************************************************************/
extern void registerGenericTypes();
}

/**********************************************************************/
Q_DECLARE_METATYPE(QHttp::stuTable)
Q_DECLARE_METATYPE(QHttp::HEADERS_t)
Q_DECLARE_METATYPE(QHttp::COOKIES_t)
Q_DECLARE_METATYPE(QHttp::JWT_t)
Q_DECLARE_METATYPE(QHttp::EncodedJWT_t)
Q_DECLARE_METATYPE(QHttp::JSON_t)
Q_DECLARE_METATYPE(QHttp::ExtraPath_t)
Q_DECLARE_METATYPE(QHttp::RemoteIP_t)
Q_DECLARE_METATYPE(QHttp::MD5_t)
Q_DECLARE_METATYPE(QHttp::IPv4_t)
Q_DECLARE_METATYPE(QHttp::Email_t)
Q_DECLARE_METATYPE(QHttp::Mobile_t)
Q_DECLARE_METATYPE(QHttp::Sheba_t)
Q_DECLARE_METATYPE(QHttp::ISO639_2_t)
Q_DECLARE_METATYPE(QHttp::Date_t)
Q_DECLARE_METATYPE(QHttp::Time_t)
Q_DECLARE_METATYPE(QHttp::DateTime_t)
Q_DECLARE_METATYPE(QHttp::Base64Image_t)


#endif // QHTTP_GENERICTYPES_H
