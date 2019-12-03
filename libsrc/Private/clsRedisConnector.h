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

#ifndef QHTTP_CLSREDISCONNECTOR_H
#define QHTTP_CLSREDISCONNECTOR_H

#ifdef QHTTP_REDIS_PROTOCOL

#include "Private/intfCacheConnector.hpp"

class redisContext;

namespace QHttp {
namespace Private{

class clsRedisConnector : public intfCacheConnector {
public:
    clsRedisConnector(const QUrl& _connector);

    void connect();
    bool reconnect();
    void setKeyValImpl(const QString& _key, const QString& _value, qint32 _ttl);
    QString getValueImpl(const QString& _key);

private:
    QScopedPointer<redisContext> Connection;
};

}
}
#endif //QHTTP_REDIS_PROTOCOL

#endif // QHTTP_CLSREDISCONNECTOR_H
