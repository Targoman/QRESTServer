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

#ifndef QHTTP_CLSJWT_H
#define QHTTP_CLSJWT_H

#include <QMessageAuthenticationCode>
#include <QJsonDocument>
#include <QJsonObject>

namespace QHttp {
namespace Private{

class QJWT
{
public:
    static QString createSigned(QJsonObject _payload, QJsonObject _privatePayload = QJsonObject(), const qint32 _expiry = -1, const QString& _sessionID = QString());
    static QJsonObject verifyReturnPayload(const QString& _jwt);

private:
    static const QByteArray hash(const QByteArray& _data);
};

}
}

#endif // QHTTP_CLSJWT_H
