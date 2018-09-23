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

#include <QMap>
#include <QMutex>

#include "Private/RESTAPIRegistry.h"

namespace QHttp {
namespace Private {

/***********************************************************************************************/

#define DO_ON_TYPE(_typeName, _baseType) DO_ON_TYPE_PROXY(_baseType, IGNORE_TYPE_##_typeName)
#define DO_ON_TYPE_SELECTOR(_1,_2,N,...) N
#define DO_ON_TYPE_PROXY(_type, ...) DO_ON_TYPE_SELECTOR(__VA_ARGS__, DO_ON_TYPE_IGNORED, DO_ON_TYPE_VALID)(_type)
#define DO_ON_TYPE_IGNORED(_baseType) nullptr

#define DO_ON_TYPE_VALID(_baseType)  new tmplAPIArg<_baseType>(TARGOMAN_M2STR(_baseType))
#define MAKE_INFO_FOR_VALID_METATYPE(_typeName, _id, _baseType) { _id, { DO_ON_TYPE(_typeName, _baseType) }},
#define MAKE_INVALID_METATYPE(_typeName, _id, _baseType) { _id, { nullptr }},

#define IGNORE_TYPE_Void ,
#define IGNORE_TYPE_QByteArray ,
#define IGNORE_TYPE_QBitArray ,
#define IGNORE_TYPE_QLocale ,
#define IGNORE_TYPE_QRect ,
#define IGNORE_TYPE_QRectF ,
#define IGNORE_TYPE_QSize ,
#define IGNORE_TYPE_QSizeF ,
#define IGNORE_TYPE_QLine ,
#define IGNORE_TYPE_QLineF ,
#define IGNORE_TYPE_QPoint ,
#define IGNORE_TYPE_QPointF ,
#define IGNORE_TYPE_QEasingCurve ,
#define IGNORE_TYPE_QModelIndex ,
#define IGNORE_TYPE_QJsonValue ,
#define IGNORE_TYPE_QJsonObject ,
#define IGNORE_TYPE_QJsonArray ,
#define IGNORE_TYPE_QJsonDocument ,
#define IGNORE_TYPE_QPersistentModelIndex ,
#define IGNORE_TYPE_QPersistentModelIndex ,
#define IGNORE_TYPE_Nullptr ,
#define IGNORE_TYPE_QVariantHash ,
#define IGNORE_TYPE_QByteArrayList ,

const QMap<int, intfAPIArgManipulator*> MetaTypeInfoMap = {
    QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(MAKE_INFO_FOR_VALID_METATYPE)
    QT_FOR_EACH_STATIC_PRIMITIVE_POINTER(MAKE_INVALID_METATYPE)
    QT_FOR_EACH_STATIC_CORE_CLASS(MAKE_INFO_FOR_VALID_METATYPE)
    QT_FOR_EACH_STATIC_CORE_POINTER(MAKE_INVALID_METATYPE)
    QT_FOR_EACH_STATIC_CORE_TEMPLATE(MAKE_INFO_FOR_VALID_METATYPE)
    QT_FOR_EACH_STATIC_GUI_CLASS(MAKE_INVALID_METATYPE)
    QT_FOR_EACH_STATIC_WIDGETS_CLASS(MAKE_INVALID_METATYPE)
};

QList<intfAPIArgManipulator*> gOrderedMetaTypeInfo;
QList<intfAPIArgManipulator*> gUserDefinedTypesInfo;

/***********************************************************************************************/
void RESTAPIRegistry::registerRESTAPI(intfRESTAPIHolder* _module, const QMetaMethod& _method){
    if(gOrderedMetaTypeInfo.isEmpty()){
        gOrderedMetaTypeInfo.reserve(MetaTypeInfoMap.lastKey());

        for(auto MetaTypeInfoMapIter = MetaTypeInfoMap.begin();
            MetaTypeInfoMapIter != MetaTypeInfoMap.end();
            ++MetaTypeInfoMapIter){
            for(int i = 0; i< MetaTypeInfoMapIter.key() - gOrderedMetaTypeInfo.size(); ++i)
                gOrderedMetaTypeInfo.append(nullptr);
            gOrderedMetaTypeInfo.append(MetaTypeInfoMapIter.value());
        }
    }
    if ((_method.name().startsWith("api") == false &&
         _method.name().startsWith("asyncApi") == false)||
        _method.typeName() == NULL)
        return;

    try{
        RESTAPIRegistry::validateMethodInputAndOutput(_method);
        QString MethodName = _method.name().constData();
        MethodName = MethodName.mid(MethodName.indexOf("api",0,Qt::CaseInsensitive) + 3);

        if(MethodName.startsWith("GET"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "GET", MethodName.at(3).toLower() + MethodName.mid(4));
        else if(MethodName.startsWith("POST"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "POST", MethodName.at(4).toLower() + MethodName.mid(5));
        else if(MethodName.startsWith("PUT"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "PUT", MethodName.at(3).toLower() + MethodName.mid(4));
        else if(MethodName.startsWith("PATCH"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "PATCH", MethodName.at(5).toLower() + MethodName.mid(6));
        else if (MethodName.startsWith("DELETE"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "DELETE", MethodName.at(6).toLower() + MethodName.mid(7));
        else if (MethodName.startsWith("UPDATE"))
            RESTAPIRegistry::addRegistryEntry(_module, _method, "PATCH", MethodName.at(6).toLower() + MethodName.mid(7));
        else if (MethodName.startsWith("WS")){
#ifdef QHTTP_ENABLE_WEBSOCKET
            RESTAPIRegistry::addRegistryEntry(_module, _method, "", MethodName.at(6).toLower() + MethodName.mid(7));
#else
            throw exRESTRegistry("Websockets are not enabled in this QRestServer please compile with websockets support");
#endif
        }else{
            RESTAPIRegistry::addRegistryEntry(_module, _method, "GET", MethodName.at(0).toLower() + MethodName.mid(1));
            RESTAPIRegistry::addRegistryEntry(_module, _method, "POST", MethodName.at(0).toLower() + MethodName.mid(1));
        }
    }catch(Targoman::Common::exTargomanBase& ex){
        TargomanError("Fatal Error: "<<ex.what());
        throw;
    }
}

QStringList RESTAPIRegistry::registeredAPIs(const QString &_module, bool _showParams, bool _showTypes, bool _prettifyTypes){
    if(_showParams == false)
        return RESTAPIRegistry::Registry.keys();

    static auto type2Str = [_prettifyTypes](int _typeID) {
        if(_prettifyTypes == false || _typeID > 1023)
            return QString(QMetaType::typeName(_typeID));

        return gOrderedMetaTypeInfo.at(_typeID)->PrettyTypeName;
    };

    QMap<QString, QString> Methods;
    foreach(const QString& Key, RESTAPIRegistry::Registry.keys()){
        QString Path = Key.split(" ").last();
        if(Path.startsWith(_module) == false)
            continue;

        clsAPIObject* APIObject = RESTAPIRegistry::Registry.value(Key);
        QStringList Parameters;
        for(int i=0; i< APIObject->BaseMethod.parameterCount(); ++i){
            Parameters.append((_showTypes ? type2Str(APIObject->BaseMethod.parameterType(i)) + " " : "" ) + (
                                  APIObject->BaseMethod.parameterNames().at(i).startsWith('_') ?
                                      APIObject->BaseMethod.parameterNames().at(i).mid(1) :
                                      APIObject->BaseMethod.parameterNames().at(i)) + (
                                  APIObject->RequiredParamsCount <= i ? "=optional" : ""));
        }

        Methods.insert(Path + Key.split(" ").first(),
                       QString("%1(%2) %3 %4").arg(
                           Key).arg(
                           Parameters.join(", ")).arg(
                           _showTypes ? QString("-> %1").arg(
                                            type2Str(APIObject->BaseMethod.returnType())) : "").arg(
                           APIObject->IsAsync ? "[async]" : ""
                                                )
                       );
    }

    return Methods.values();
}

QString RESTAPIRegistry::isValidType(int _typeID, bool _validate4Input){
    if(_typeID == 0 || _typeID == QMetaType::User || _typeID == 1025)
        return  "is not registered with Qt MetaTypes";
    if(_typeID < QHTTP_BASE_USER_DEFINED_TYPEID && (_typeID >= gOrderedMetaTypeInfo.size() || gOrderedMetaTypeInfo.at(_typeID) == nullptr))
        return "is complex type and not supported";

    if(_typeID >= QHTTP_BASE_USER_DEFINED_TYPEID){
        if((_typeID - QHTTP_BASE_USER_DEFINED_TYPEID >= gUserDefinedTypesInfo.size() ||
            gUserDefinedTypesInfo.at(_typeID - QHTTP_BASE_USER_DEFINED_TYPEID) == nullptr ||
            strcmp(gUserDefinedTypesInfo.at(_typeID - QHTTP_BASE_USER_DEFINED_TYPEID)->RealTypeName, QMetaType::typeName(_typeID))))
            return "is user defined but not registered";

        if(_validate4Input){
            if(!gUserDefinedTypesInfo.at(_typeID - QHTTP_BASE_USER_DEFINED_TYPEID)->hasFromVariantMethod())
                return "has no fromVariant lambda so can not be used as input";
        }else{
            if(!gUserDefinedTypesInfo.at(_typeID - QHTTP_BASE_USER_DEFINED_TYPEID)->hasToVariantMethod())
                return "has no toVariant lambda so can not be used as output";
        }
    }
    return "";
}

void RESTAPIRegistry::validateMethodInputAndOutput(const QMetaMethod &_method){
    if(_method.parameterCount() > 9)
        throw exRESTRegistry("Unable to register methods with more than 9 input args");

    QString ErrMessage;
    if ((ErrMessage = RESTAPIRegistry::isValidType(_method.returnType(), false)).size())
        throw exRESTRegistry(QString("Invalid return type(%1): %2").arg(_method.typeName()).arg(ErrMessage));

    ErrMessage.clear();

    for(int i=0; i<_method.parameterCount(); ++i){
        if ((ErrMessage = RESTAPIRegistry::isValidType(_method.parameterType(i), true)).size())
            throw exRESTRegistry(QString("Invalid parameter (%1 %2): %3").arg(
                                     _method.parameterTypes().at(i).constData()).arg(
                                     _method.parameterNames().at(i).constData()).arg(
                                     ErrMessage));

        for(int j=i+1; j<_method.parameterCount(); ++j){
            if(_method.parameterNames().at(j) == _method.parameterNames().at(i))
                throw exRESTRegistry(QString("Invalid duplicate parameter name %1 at %2 and %3").arg(
                                         _method.parameterNames().at(i).constData()).arg(
                                         i).arg(
                                         j
                                         ));
        }
    }

    //TODO if either return or parameter type is of user defined types check they can be converted to Json
}

const char* CACHE_INTERNAL = "CACHEABLE_";
const char* CACHE_CENTRAL  = "CENTRALCACHE_";

void RESTAPIRegistry::addRegistryEntry(intfRESTAPIHolder *_module, const QMetaMethod &_method, const QString &_httpMethod, const QString &_methodName){
    auto cache4Secs = [](const QMetaMethod& _method, const char* _type){
        if(_method.tag() == NULL || _method.tag()[0] == '\0')
            return 0;
        QString Tag = _method.tag();
        if(Tag.startsWith(_type) && Tag.size () > 12){
            Tag = Tag.mid(10);
            if(_type == CACHE_INTERNAL && Tag == "INF")
                return -1;
            char Type = Tag.rbegin()->toLatin1();
            int  Number = Tag.mid(0,Tag.size() -1).toUShort();
            switch(Type){
            case 'S': return Number;
            case 'M': return Number * 60;
            case 'H': return Number * 3600;
            default:
                throw exRESTRegistry("Invalid CACHE numer or type defined for api: " + _method.methodSignature());
            }
        }else
            throw exRESTRegistry("Invalid tag defined for api: " + _method.methodSignature());
    };

    QString MethodKey = RESTAPIRegistry::makeRESTAPIKey(_httpMethod, "/" + _module->moduleFullName().replace("::", "/")+ '/' + _methodName);

    if(RESTAPIRegistry::Registry.contains(MethodKey)){
        if(RESTAPIRegistry::Registry.value(MethodKey)->isPolymorphic(_method))
            throw exRESTRegistry(QString("Polymorphism is not supported: %1").arg(_method.methodSignature().constData()));
        RESTAPIRegistry::Registry.value(MethodKey)->updateDefaultValues(_method);
    }else{
        if(cache4Secs(_method, CACHE_INTERNAL) > 0 && cache4Secs(_method, CACHE_CENTRAL) >0)
            throw exRESTRegistry("Both internal and central cache can not be defined on an API");

        RESTAPIRegistry::Registry.insert(MethodKey,
                                         new clsAPIObject(_module,
                                                          _method,
                                                          QString(_method.name()).startsWith("async"),
                                                          cache4Secs(_method, CACHE_INTERNAL),
                                                          cache4Secs(_method, CACHE_CENTRAL)
                                                          ));
    }
}

Cache_t InternalCache::Cache;
QMutex InternalCache::Lock;
QScopedPointer<intfCacheConnector> CentralCache::Connector;

}
}
