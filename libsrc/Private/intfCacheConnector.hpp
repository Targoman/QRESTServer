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

#ifndef QHTTP_INTFCACHECONNECTOR_HPP
#define QHTTP_INTFCACHECONNECTOR_HPP

#include <QUrl>
#include <QVariant>
#include "libTargomanCommon/exTargomanBase.h"
#include "Private/intfAPIArgManipulator.h"
#include "Private/Configs.hpp"

namespace QHttp {
namespace Private {
TARGOMAN_ADD_EXCEPTION_HANDLER(exCacheConnector, Targoman::Common::exTargomanBase);

class intfCacheConnector{
public:
    intfCacheConnector(const QUrl& _connector) :
        ConnectorURL(_connector)
    {}
    virtual ~intfCacheConnector();

    virtual void connect() = 0;
    void setKeyVal(const QString& _key, const QVariant& _value, qint32 _ttl){
        if(_value.type() >= QHTTP_BASE_USER_DEFINED_TYPEID)
            this->setKeyValImpl(_key,
                                gUserDefinedTypesInfo.at(_value.type() - QHTTP_BASE_USER_DEFINED_TYPEID)->toString(_value),
                                _ttl);
        else
            this->setKeyValImpl(_key, _value.toString(), _ttl);
    }

    QVariant getValue(const QString& _key){
        return this->getValueImpl (_key);
    }

private:
    virtual void setKeyValImpl(const QString& _key, const QString& _value, qint32 _ttl) = 0;
    virtual QString getValueImpl(const QString& _key) = 0;

protected:
    QUrl ConnectorURL;
};


}
}

#endif // QHTTP_INTFCACHECONNECTOR_HPP
