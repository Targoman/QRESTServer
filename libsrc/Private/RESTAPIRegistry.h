/*******************************************************************************
 * QRESTServer a lean and mean Qt/C++ based REST server                     *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * QRESTServer is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * QRESTServer is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU AFFERO GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE    *
 * along with QRESTServer. If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                             *
 *******************************************************************************/
/**
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 */

#ifndef QHTTP_PRIVATE_RESTAPIREGISTRY_HPP
#define QHTTP_PRIVATE_RESTAPIREGISTRY_HPP

#include <QGenericArgument>
#include <QMetaMethod>
#include "QHttp/intfRESTAPIHolder.h"

#include "Private/Configs.hpp"
#include "Private/clsAPIObject.hpp"

namespace QHttp {
namespace Private {

TARGOMAN_ADD_EXCEPTION_HANDLER(exRESTRegistry, Targoman::Common::exTargomanBase);

class RESTAPIRegistry
{
public:
    static inline QString makeRESTAPIKey(const QString& _httpMethod, const QString& _path){
        return  _httpMethod.toUpper() + " " +_path;
    }
    static inline clsAPIObject* getAPIObject(const QString _httpMethod, const QString& _path){
        return RESTAPIRegistry::Registry.value(RESTAPIRegistry::makeRESTAPIKey(_httpMethod, _path));
    }
    static inline QGenericArgument jsonToGenericArgument(const char* _type, const QString& _value, void* _storage){
        fnDeserializer_t fnDeserializer = RESTAPIRegistry::RegisteredDeserializers.value(_type);
        if(!fnDeserializer)
            throw exRESTRegistry(QString("Type <%1> was not registered").arg(_type));
        fnDeserializer(_storage, _value);
        return QGenericArgument(_type, _storage);
    }

    static void registerRESTAPI(intfRESTAPIHolder* _module, const QMetaMethod& _method);
    static QStringList registeredAPIs(const QString &_module, bool _showParams = false, bool _showTypes = false, bool _prettifyTypes = true);

private:
    static inline QString isValidType(int _typeID);
    static void validateMethodInputAndOutput(const QMetaMethod& _method);
    static void addRegistryEntry(intfRESTAPIHolder* _module, const QMetaMethod& _method, const QString& _httpMethod, const QString& _methodName);

private:
    static QHash<QString, clsAPIObject*>  Registry;
    static QHash<QString, fnDeserializer_t> RegisteredDeserializers;
};

}
}


#endif // QHTTP_PRIVATE_RESTAPIREGISTRY_HPP
