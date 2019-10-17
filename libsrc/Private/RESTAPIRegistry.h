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

#ifndef QHTTP_PRIVATE_RESTAPIREGISTRY_H
#define QHTTP_PRIVATE_RESTAPIREGISTRY_H

#include <QGenericArgument>
#include <QMetaMethod>
#include "QHttp/intfRESTAPIHolder.h"

#include "Private/Configs.hpp"
#include "Private/clsAPIObject.hpp"
#include "Private/NumericTypes.hpp"

namespace QHttp {
namespace Private {

TARGOMAN_ADD_EXCEPTION_HANDLER(exRESTRegistry, Targoman::Common::exTargomanBase);

class RESTAPIRegistry
{
public:
    static inline QString makeRESTAPIKey(const QString& _httpMethod, const QString& _path){
        return  _httpMethod.toUpper() + " " + (_path.endsWith('/') ? _path.mid(0, _path.size() - 1) : _path);
    }

    static inline clsAPIObject*
    getAPIObject(const QString _httpMethod, const QString& _path){
        return RESTAPIRegistry::Registry.value(RESTAPIRegistry::makeRESTAPIKey(_httpMethod, _path));
    }
#ifdef QHTTP_ENABLE_WEBSOCKET
    static inline clsAPIObject* getWSAPIObject(const QString& _path){
        return RESTAPIRegistry::WSRegistry.value(RESTAPIRegistry::makeRESTAPIKey("WS", _path));
    }
#endif

    static void registerRESTAPI(intfRESTAPIHolder* _module, const QMetaMethod& _method);
    static QStringList registeredAPIs(const QString& _module, bool _showParams = false, bool _showTypes = false, bool _prettifyTypes = true);
    static QJsonObject retriveOpenAPIJson();

private:
    static inline QString isValidType(int _typeID, bool _validate4Input);
    static void validateMethodInputAndOutput(const QMetaMethod& _method);
    static void addRegistryEntry(QHash<QString, clsAPIObject*>& _registry, intfRESTAPIHolder* _module, const QMetaMethodExtended& _method, const QString& _httpMethod, const QString& _methodName);
    static int  getCacheSeconds(const QMetaMethod& _method, const char* _type);
    static QMap<QString, QString> extractMethods(QHash<QString, clsAPIObject*>& _registry, const QString& _module, bool _showTypes, bool _prettifyTypes);

private:
    static QHash<QString, clsAPIObject*>  Registry;
    static QJsonObject CachedOpenAPI;

#ifdef QHTTP_ENABLE_WEBSOCKET
    static QHash<QString, clsAPIObject*>  WSRegistry;
#endif
};

}
}

#endif // QHTTP_PRIVATE_RESTAPIREGISTRY_H
