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

#ifndef QHTTP_PRIVATE_CLSAPIOBJECT_HPP
#define QHTTP_PRIVATE_CLSAPIOBJECT_HPP

#include <QGenericArgument>
#include <QMetaMethod>

#include "QHttp/intfRESTAPIHolder.h"

#include "Private/Configs.hpp"
#include "Private/RESTAPIRegistry.h"

namespace QHttp {
namespace Private {

class clsAPIObject : public intfAPIObject, public QObject
{
public:
    clsAPIObject(intfRESTAPIHolder* _module, QMetaMethod _method, bool _async, qint32 _cache4) :
        QObject(_module),
        BaseMethod(_method),
        IsAsync(_async),
        Cache4Secs(_cache4),
        RequiredParamsCount(_method.parameterCount())
    {
        foreach(const QByteArray& ParamName, _method.parameterNames())
            this->ParamNames.append(ParamName.startsWith('_') ? ParamName.mid(1) : ParamName);
    }

    inline QVariant invoke(const QStringList& _args, QList<QPair<QString, QString>> _bodyArgs) const{
        Q_ASSERT_X(this->parent(), "parent module", "Parent module not found to invoke method");

        if(_args.size() + _bodyArgs.size() < this->RequiredParamsCount)
            throw exHTTPBadRequest("Not enough arguments");

        QVariantList Arguments;

        qint8 FirstArgumentWithValue = -1;
        qint8 LastArgumentWithValue = -1;

        for(int i=0; i< this->ParamNames.count(); ++i ){
            bool ParamNotFound = true;
            QVariant ArgumentValue;

            static auto parseArgValue = [this, ArgumentValue](const QString& _paramNamne, const QString& _value) -> QVariant {
                if((_value.startsWith('[') && _value.endsWith(']')) ||
                   (_value.startsWith('{') && _value.endsWith('}'))){
                    QJsonParseError Error;
                    QJsonDocument JSON = QJsonDocument::fromJson(_value.toUtf8(), &Error);
                    if(JSON.isNull())
                        throw exHTTPBadRequest(QString("Invalid value for %1: %2").arg(_paramNamne).arg(Error.errorString()));
                    return JSON.toVariant();
                }else{
                    return _value;
                }
            };

            foreach (const QString& Arg, _args){
                if(Arg.startsWith(this->ParamNames.at(i)+ '=')){
                    ParamNotFound = false;
                    ArgumentValue = parseArgValue(this->ParamNames.at(i), Arg.mid(Arg.indexOf('=') + 1));
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

                Arguments.append(ArgumentValue);
            }else{
                Q_ASSERT(this->BaseMethod.parameterType(i) < gOrderedMetaTypeInfo.size());
                Q_ASSERT(gOrderedMetaTypeInfo.at(this->BaseMethod.parameterType(i)) != nullptr);

                Arguments.append(ArgumentValue);
            }
        }

        if(FirstArgumentWithValue < 0)
            Arguments.clear();
        else if (LastArgumentWithValue < Arguments.size() - 1)
            Arguments = Arguments.mid(0, LastArgumentWithValue + 1);

        if(this->BaseMethod.returnType() >= QHTTP_BASE_USER_DEFINED_TYPEID){
            Q_ASSERT(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID < gOrderedMetaTypeInfo.size());
            Q_ASSERT(gUserDefinedTypesInfo.at(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID) != nullptr);

            return gUserDefinedTypesInfo.at(this->BaseMethod.returnType() - QHTTP_BASE_USER_DEFINED_TYPEID)->invokeMethod(this, Arguments);
        }else{
            Q_ASSERT(this->BaseMethod.returnType() < gOrderedMetaTypeInfo.size());
            Q_ASSERT(gOrderedMetaTypeInfo.at(this->BaseMethod.returnType()) != nullptr);

            return gOrderedMetaTypeInfo.at(this->BaseMethod.returnType())->invokeMethod(this, Arguments);
        }
    }

#define MAKE_ARG_AT(_i) gOrderedMetaTypeInfo.at(this->BaseMethod.parameterType(_i))->makeGenericArgument(_arguments.at(_i), this->ParamNames.at(_i), &ArgStorage[_i])
#define CLEAN_ARG_AT(_i) gOrderedMetaTypeInfo.at(this->BaseMethod.parameterType(_i))->cleanup(ArgStorage[_i])

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
            case 0: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg);break;
            case 1: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0));break;
            case 2: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0), MAKE_ARG_AT(1));break;
            case 3: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2));break;
            case 4: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3));break;
            case 5: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4));break;
            case 6: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4),
                                                              MAKE_ARG_AT(5));break;
            case 7: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4),
                                                              MAKE_ARG_AT(5),MAKE_ARG_AT(6));break;
            case 8: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4),
                                                              MAKE_ARG_AT(5),MAKE_ARG_AT(6),MAKE_ARG_AT(7));break;
            case 9: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                              _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4),
                                                              MAKE_ARG_AT(5),MAKE_ARG_AT(6),MAKE_ARG_AT(7),MAKE_ARG_AT(8));break;
            case 10: InvocationResult = InvokableMethod.invoke(this->parent(), this->IsAsync ? Qt::QueuedConnection : Qt::DirectConnection,
                                                               _returnArg, MAKE_ARG_AT(0),MAKE_ARG_AT(1),MAKE_ARG_AT(2),MAKE_ARG_AT(3),MAKE_ARG_AT(4),
                                                               MAKE_ARG_AT(5),MAKE_ARG_AT(6),MAKE_ARG_AT(7),MAKE_ARG_AT(8),MAKE_ARG_AT(9));break;
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

private:
    void updateDefaultValues(QMetaMethod _method){
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
    QList<QByteArray>  ParamNames;
    quint8        RequiredParamsCount;

    friend class RESTAPIRegistry;
};

}
}
#endif // QHTTP_PRIVATE_CLSAPIOBJECT_HPP
