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

#ifndef QHTTP_PRIVATE_CLSAPIOBJECT_HPP
#define QHTTP_PRIVATE_CLSAPIOBJECT_HPP

#include <QGenericArgument>
#include <QMetaMethod>

#include "QHttp/intfRESTAPIHolder.h"

#include "Private/Configs.hpp"
#include "Private/RESTAPIRegistry.h"
#include "Private/APICache.hpp"

namespace QHttp {
namespace Private {

class clsAPIObject : public intfAPIObject, public QObject
{
public:
    clsAPIObject(intfRESTAPIHolder* _module, QMetaMethod _method, bool _async, qint32 _cache4Internal, qint32 _cache4Central) :
        QObject(_module),
        BaseMethod(_method),
        IsAsync(_async),
        Cache4Secs(_cache4Internal),
        Cache4SecsCentral(_cache4Central),
        RequiredParamsCount(_method.parameterCount())
    {
        foreach(const QByteArray& ParamName, _method.parameterNames())
            this->ParamNames.append(ParamName.startsWith('_') ? ParamName.mid(1) : ParamName);
    }

    inline QString makeCacheKey(const QVariantList& _args) const{
        return (this->BaseMethod.name() + QJsonValue::fromVariant(_args).toString().toUtf8()).constData();
    }


    inline QVariant invoke(const QStringList& _args, QList<QPair<QString, QString>> _bodyArgs = QList<QPair<QString, QString>>()) const{
        Q_ASSERT_X(this->parent(), "parent module", "Parent module not found to invoke method");

        if(_args.size() + _bodyArgs.size() < this->RequiredParamsCount)
            throw exHTTPBadRequest("Not enough arguments");

        QVariantList Arguments;

        qint8 FirstArgumentWithValue = -1;
        qint8 LastArgumentWithValue = -1;

        for(int i=0; i< this->ParamNames.count(); ++i ){
            bool ParamNotFound = true;
            QVariant ArgumentValue;

            static auto parseArgValue = [this, ArgumentValue](const QString& _paramName, const QString& _value) -> QVariant {
                if((_value.startsWith('[') && _value.endsWith(']')) ||
                   (_value.startsWith('{') && _value.endsWith('}'))){
                    QJsonParseError Error;
                    QJsonDocument JSON = QJsonDocument::fromJson(_value.toUtf8(), &Error);
                    if(JSON.isNull())
                        throw exHTTPBadRequest(QString("Invalid value for %1: %2").arg(_paramName).arg(Error.errorString()));
                    return JSON.toVariant();
                }else{
                    return _value;
                }
            };

            foreach (const QString& Arg, _args){
                if(Arg.startsWith(this->ParamNames.at(i)+ '=')){
                    ParamNotFound = false;
                    ArgumentValue = parseArgValue(this->ParamNames.at(i), QUrl::fromPercentEncoding(Arg.mid(Arg.indexOf('=') + 1).toUtf8()));
                    break;
                }
            }

            if(ParamNotFound)
                foreach (auto BodyArg, _bodyArgs) {
                    bool ConversionResult = true;
                    if(BodyArg.first == this->ParamNames.at(i) || (BodyArg.first.toInt(&ConversionResult) == i && ConversionResult)){
                        ParamNotFound = false;
                        ArgumentValue = parseArgValue(this->ParamNames.at(i), BodyArg.second);
                        break;
                    }
                }

            if(ParamNotFound){
                if(i < this->RequiredParamsCount)
                    throw exHTTPBadRequest(QString("Required parameter <%1> not specified").arg(this->ParamNames.at(i).constData()));
                else
                    Arguments.append(QVariant());
                continue;
            }else if(ArgumentValue.isValid() == false)
                throw exHTTPBadRequest(QString("Invalid value for %1: no conversion to QVariant defined").arg(this->ParamNames.at(i).constData()));

            if(FirstArgumentWithValue < 0)
                FirstArgumentWithValue = i;
            LastArgumentWithValue = i;

            if(this->BaseMethod.parameterType(i) >= QHTTP_BASE_USER_DEFINED_TYPEID){
                Q_ASSERT(this->BaseMethod.parameterType(i) - QHTTP_BASE_USER_DEFINED_TYPEID < gOrderedMetaTypeInfo.size());
                Q_ASSERT(gUserDefinedTypesInfo.at(this->BaseMethod.parameterType(i) - QHTTP_BASE_USER_DEFINED_TYPEID) != nullptr);

                Arguments.push_back(
                            ArgumentValue);
            }else{
                Q_ASSERT(this->BaseMethod.parameterType(i) < gOrderedMetaTypeInfo.size());
                Q_ASSERT(gOrderedMetaTypeInfo.at(this->BaseMethod.parameterType(i)) != nullptr);

                Arguments.push_back(ArgumentValue);
            }
        }

        if(FirstArgumentWithValue < 0)
            Arguments.clear();
        else if (LastArgumentWithValue < Arguments.size() - 1)
            Arguments = Arguments.mid(0, LastArgumentWithValue + 1);

        QVariant Result;
        QString  CacheKey;
        if(this->Cache4Secs != 0 || this->Cache4SecsCentral != 0)
            CacheKey = this->makeCacheKey(Arguments);

        if(this->Cache4Secs != 0){
            QVariant CachedValue =  InternalCache::storedValue(CacheKey);
            if(CachedValue.isValid()){
                gServerStats.APIInternalCacheStats[this->BaseMethod.name()].inc();
                return CachedValue;
            }
        }

        if(this->Cache4SecsCentral){
            QVariant CachedValue =  CentralCache::storedValue(CacheKey);
            if(CachedValue.isValid()){
                gServerStats.APICentralCacheStats[this->BaseMethod.name()].inc();
                return CachedValue;
            }
        }

        if(this->BaseMethod.returnType() >= QHTTP_BASE_USER_DEFINED_TYPEID){
            Q_ASSERT(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID < gOrderedMetaTypeInfo.size());
            Q_ASSERT(gUserDefinedTypesInfo.at(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID) != nullptr);

            Result = gUserDefinedTypesInfo.at(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID)->invokeMethod(this, Arguments);
        }else{
            Q_ASSERT(this->BaseMethod.returnType() < gOrderedMetaTypeInfo.size());
            Q_ASSERT(gOrderedMetaTypeInfo.at(this->BaseMethod.returnType()) != nullptr);

            Result = gOrderedMetaTypeInfo.at(this->BaseMethod.returnType())->invokeMethod(this, Arguments);
        }

        if(this->Cache4Secs != 0)
            InternalCache::setValue(CacheKey, Result, this->Cache4Secs);
        else if(this->Cache4SecsCentral != 0)
            CentralCache::setValue(CacheKey, Result, this->Cache4SecsCentral);

        gServerStats.APICallsStats[this->BaseMethod.name()].inc();
        return Result;
    }

#define USE_ARG_AT(_i) \
    InvokableMethod.parameterType(_i) < QHTTP_BASE_USER_DEFINED_TYPEID ? \
    gOrderedMetaTypeInfo.at(InvokableMethod.parameterType(_i))->makeGenericArgument(_arguments.at(_i), this->ParamNames.at(_i), &ArgStorage[_i]) : \
    gUserDefinedTypesInfo.at(InvokableMethod.parameterType(_i) - QHTTP_BASE_USER_DEFINED_TYPEID)->makeGenericArgument(_arguments.at(_i), this->ParamNames.at(_i), &ArgStorage[_i])

#define CLEAN_ARG_AT(_i) \
    InvokableMethod.parameterType(_i) < QHTTP_BASE_USER_DEFINED_TYPEID ? \
    gOrderedMetaTypeInfo.at(InvokableMethod.parameterType(_i))->cleanup(ArgStorage[_i]) : \
    gUserDefinedTypesInfo.at(InvokableMethod.parameterType(_i) - QHTTP_BASE_USER_DEFINED_TYPEID)->cleanup(ArgStorage[_i])

    void invokeMethod(const QVariantList& _arguments, QGenericReturnArgument _returnArg) const{
        bool InvocationResult= true;
        QMetaMethod InvokableMethod;
        if(_arguments.size() == this->ParamNames.size())
            InvokableMethod = this->BaseMethod;
        else
            InvokableMethod = this->LessArgumentMethods.at(this->ParamNames.size() - _arguments.size() - 1);

        void* ArgStorage[_arguments.size()];

        try{
            switch(_arguments.size()){
            case  0: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg);break;
            case  1: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0));break;
            case  2: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0), USE_ARG_AT(1));break;
            case  3: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2));break;
            case  4: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3));break;
            case  5: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4));break;
            case  6: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4),
                                                               USE_ARG_AT(5));break;
            case  7: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4),
                                                               USE_ARG_AT(5),USE_ARG_AT(6));break;
            case  8: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4),
                                                               USE_ARG_AT(5),USE_ARG_AT(6),USE_ARG_AT(7));break;
            case  9: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4),
                                                               USE_ARG_AT(5),USE_ARG_AT(6),USE_ARG_AT(7),USE_ARG_AT(8));break;
            case 10: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, USE_ARG_AT(0),USE_ARG_AT(1),USE_ARG_AT(2),USE_ARG_AT(3),USE_ARG_AT(4),
                                                               USE_ARG_AT(5),USE_ARG_AT(6),USE_ARG_AT(7),USE_ARG_AT(8),USE_ARG_AT(9));break;
            }

            if (InvocationResult == false)
                throw exHTTPInternalServerError(QString("Unable to invoke method"));

            for(int i=0; i< _arguments.size(); ++i)
                CLEAN_ARG_AT(i);
        }catch(...){
            for(int i=0; i< _arguments.size(); ++i)
                CLEAN_ARG_AT(i);
            throw;
        }
    }

    bool isPolymorphic(const QMetaMethod& _method){
        if(_method.parameterCount() == 0)
            return false;
        for(int i=0; i< qMin(_method.parameterCount(), this->BaseMethod.parameterCount()); ++i)
            if(this->BaseMethod.parameterType(i) != _method.parameterType(i))
                return true;
        return false;
    }

private:
    void updateDefaultValues(const QMetaMethod& _method){
        if(_method.parameterNames().size() < this->RequiredParamsCount){
            this->RequiredParamsCount = _method.parameterNames().size();
            this->LessArgumentMethods.append(_method);
        }
    }

private:
    QMetaMethod   BaseMethod;
    QList<QMetaMethod> LessArgumentMethods;
    bool          IsAsync;
    qint32        Cache4Secs;
    qint32        Cache4SecsCentral;
    QList<QByteArray>  ParamNames;
    quint8        RequiredParamsCount;

    friend class RESTAPIRegistry;
};

}
}
#endif // QHTTP_PRIVATE_CLSAPIOBJECT_HPP
