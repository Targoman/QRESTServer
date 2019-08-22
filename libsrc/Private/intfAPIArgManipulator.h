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

#ifndef QHTTP_INTFAPIARGMANIPULATOR_H
#define QHTTP_INTFAPIARGMANIPULATOR_H

#include "QHttp/HTTPExceptions.h"

namespace QHttp {
namespace Private{

class intfCacheConnector;
/**********************************************************************/
class intfAPIObject{
public:
    virtual void invokeMethod(const QVariantList& _arguments, QGenericReturnArgument _returnArg) const = 0;
};

/**********************************************************************/
class intfAPIArgManipulator{
public:
    intfAPIArgManipulator(const QString& _realTypeName);

    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage) = 0;
    virtual QVariant invokeMethod(const intfAPIObject* _apiObject, const QVariantList& _arguments) = 0;
    virtual void cleanup (void* _argStorage) = 0;
    virtual bool hasFromVariantMethod() = 0;
    virtual bool hasToVariantMethod() = 0;
    virtual QString toString(const QVariant _val) = 0;

    QString     PrettyTypeName;
    char*       RealTypeName;
};

/**********************************************************************/
template<typename _itmplType>
class tmplAPIArg : public intfAPIArgManipulator{
public:
    tmplAPIArg(const QString& _typeName,
               std::function<QVariant(const _itmplType& _value)> _toVariant = {},
               std::function<_itmplType(const QVariant& _value)> _fromVariant = {}) :
        intfAPIArgManipulator(_typeName),
        toVariant(_toVariant),
        fromVariant(_fromVariant)
    {}
    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage) final{
        *_argStorage = nullptr;
        if(this->fromVariant == nullptr && !_val.canConvert<_itmplType>())
                throw exHTTPBadRequest("Invalid value specified for parameter: " + _paramName);
        *_argStorage = new _itmplType;
        *((_itmplType*)*_argStorage) =
                this->fromVariant == nullptr ? _val.value<_itmplType>() : this->fromVariant(_val);
        return QGenericArgument(this->RealTypeName, *_argStorage);
    }
    inline QVariant invokeMethod(const intfAPIObject *_apiObject, const QVariantList& _arguments) final {
           _itmplType Result;
           _apiObject->invokeMethod(_arguments,QReturnArgument<_itmplType >(this->RealTypeName, Result));
           return this->toVariant == nullptr ? QVariant::fromValue(Result) : this->toVariant(Result);
    }
    inline void cleanup (void* _argStorage) final {if(_argStorage) delete ((_itmplType*)_argStorage);}
    inline bool hasFromVariantMethod() final {return this->fromVariant != nullptr;}
    inline bool hasToVariantMethod() final {return this->toVariant != nullptr;}
    inline QString toString(const QVariant _val) {
        if(this->hasFromVariantMethod() && this->hasToVariantMethod())
            return this->toVariant(this->fromVariant(_val)).toString();
        return QString();
    }
private:
    std::function<QVariant(_itmplType _value)> toVariant;
    std::function<_itmplType(QVariant _value)> fromVariant;

    friend class intfCacheConnector;
};

/**********************************************************************/
#define QHTTP_REGISTER_METATYPE(_type, _lambdaToVariant, _lambdaFromVariant) \
    qRegisterMetaType<_type>(); \
    gUserDefinedTypesInfo.insert( \
        QMetaType::type(TARGOMAN_M2STR(_type)) - 1025, \
                        new tmplAPIArg<_type>(TARGOMAN_M2STR(_type), \
                                              _lambdaToVariant, \
                                              _lambdaFromVariant))

/** @TODO document QT_NO_CAST_FROM_ASCII */

#define QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(_numericType, _convertor) \
template<> inline QGenericArgument tmplAPIArg<_numericType>::makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage){ \
    bool Result; *_argStorage = new _numericType; *((_numericType*)*_argStorage) = _val._convertor(&Result); \
    if(!Result) throw exHTTPBadRequest("Invalid value specified for parameter: " + _paramName); \
    return QGenericArgument(this->RealTypeName, *_argStorage); \
}

QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint8, toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint16, toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint32, toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint64, toULongLong)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint8, toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint16, toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint32, toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint64, toLongLong)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qreal, toDouble)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(float, toFloat)

}
}
#endif // QHTTP_INTFAPIARGMANIPULATOR_H
