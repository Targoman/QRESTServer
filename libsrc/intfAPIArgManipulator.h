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

#ifndef QHTTP_INTFAPIARGMANIPULATOR_H
#define QHTTP_INTFAPIARGMANIPULATOR_H

#include "QHttp/HTTPExceptions.h"

namespace QHttp {

class intfCacheConnector;
/**********************************************************************/
class intfAPIObject{
public:
    virtual ~intfAPIObject();
    virtual void invokeMethod(const QVariantList& _arguments, QGenericReturnArgument _returnArg) const = 0;
};

/**********************************************************************/
class intfAPIArgManipulator{
public:
    intfAPIArgManipulator(const QString& _realTypeName);
    virtual ~intfAPIArgManipulator();

    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage) = 0;
    virtual QVariant invokeMethod(const intfAPIObject* _apiObject, const QVariantList& _arguments) = 0;
    virtual void cleanup (void* _argStorage) = 0;
    virtual bool hasFromVariantMethod() = 0;
    virtual bool hasToVariantMethod() = 0;
    virtual QString toString(const QVariant _val) = 0;
    virtual bool isIntegralType() = 0;

    QString     PrettyTypeName;
    char*       RealTypeName;
};

#define QHTTP_ADD_SIMPLE_TYPE(_type, _name) \
    class _name:public _type{public:_name(){;}_name(const _type& _other):_type(_other){;}}

#define QHTTP_ADD_COMPLEX_TYPE(_type, _name) \
    class _name:public _type{ \
        public:_name(){;}_name(const _type& _other):_type(_other){;} \
        _name fromVariant (const QVariant& _value){_type Value = _type::fromVariant (_value); return *reinterpret_cast<_name*>(&Value);}}

#define QHTTP_VALIDATION_REQUIRED_TYPE_IMPL(_type, _validationRule, _toVariant) \
    QHTTP_REGISTER_METATYPE( \
        _type, \
        [](const _type& _value) -> QVariant {return _toVariant;}, \
        [](const QVariant& _value, const QByteArray& _paramName) -> _type { \
            static QFieldValidator Validator = QFieldValidator()._validationRule; \
            if(Validator.isValid(_value, _paramName) == false) throw exHTTPBadRequest(Validator.errorMessage()); \
            _type Value; Value=_value.toString();  return  Value; \
        } \
    )
}

#define QHTTP_REGISTER_METATYPE(_type, _lambdaToVariant, _lambdaFromVariant) \
    qRegisterMetaType<_type>(); \
    QHttp::RESTServer::registerUserDefinedType(TARGOMAN_M2STR(_type), \
                                               new tmplAPIArg<_type, false>(TARGOMAN_M2STR(_type), \
                                                                     _lambdaToVariant, \
                                                                     _lambdaFromVariant))

#define QHTTP_REGISTER_ENHANCED_ENUM(_enum) \
    QHTTP_REGISTER_METATYPE( \
        _enum::Type, \
        [](_enum::Type _value) -> QVariant{return _enum::toStr(_value);}, \
        [](const QVariant& _value, const QByteArray& _paramName) -> _enum::Type { \
            _enum::Type Value = _enum::toEnum(_value.toString().toLatin1().constData()); \
            if(Value == _enum::Unknown) throw exHTTPBadRequest(QString("%1 is not a valid %2").arg( \
                        _paramName.constData()).arg( \
                        QString(TARGOMAN_M2STR(_enum)).startsWith("enu") ? QString(TARGOMAN_M2STR(_enum)).mid(3) : QString(TARGOMAN_M2STR(_enum)))); \
            return Value; \
        } \
    )


#endif // QHTTP_INTFAPIARGMANIPULATOR_H
