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

#ifndef QHTTP_TMPLAPIARG_HPP
#define QHTTP_TMPLAPIARG_HPP

#include "HTTPExceptions.h"
#include "intfAPIArgManipulator.h"

namespace QHttp {
namespace Private {
class intfCacheConnector;
}

template<typename _itmplType, enuVarComplexity _itmplVarType>
class tmplAPIArg : public intfAPIArgManipulator{
public:
    tmplAPIArg(const QString& _typeName,
               std::function<QVariant(const _itmplType& _value)> _toVariant = {},
               std::function<_itmplType(const QVariant& _value, const QByteArray& _paramName)> _fromVariant = {},
               std::function<QStringList()> _options = {}
               ) :
        intfAPIArgManipulator(_typeName),
        toVariant(_toVariant),
        fromVariant(_fromVariant),
        optionsLambda(_options)
    {}
    virtual ~tmplAPIArg(){;}

    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage) final{
        *_argStorage = nullptr;
        if(this->fromVariant == nullptr && !_val.canConvert<_itmplType>())
                throw exHTTPBadRequest("Invalid value specified for parameter: " + _paramName);
        *_argStorage = new _itmplType;
        *(reinterpret_cast<_itmplType*>(*_argStorage)) =
                this->fromVariant == nullptr ? _val.value<_itmplType>() : this->fromVariant(_val, _paramName);
        return QGenericArgument(this->RealTypeName, *_argStorage);
    }
    inline QVariant invokeMethod(const intfAPIObject *_apiObject, const QVariantList& _arguments) final {
           _itmplType Result;
           _apiObject->invokeMethod(_arguments, QReturnArgument<_itmplType >(this->RealTypeName, Result));
           return this->toVariant == nullptr ? QVariant::fromValue(Result) : this->toVariant(Result);
    }
    inline void cleanup (void* _argStorage) final {if(_argStorage) delete (reinterpret_cast<_itmplType*>(_argStorage));}
    inline bool hasFromVariantMethod() final {return this->fromVariant != nullptr;}
    inline bool hasToVariantMethod() final {return this->toVariant != nullptr;}
    inline bool isIntegralType() final { return _itmplVarType == COMPLEXITY_Integral;}
    inline QStringList options() final { return this->optionsLambda ? this->optionsLambda() : QStringList() ;}
    inline enuVarComplexity complexity() final { return _itmplVarType;}

    inline QString toString(const QVariant _val) {
        if(this->hasFromVariantMethod() && this->hasToVariantMethod())
            return this->toVariant(this->fromVariant(_val, {})).toString();
        return QString();
    }

private:
    std::function<QVariant(_itmplType _value)> toVariant;
    std::function<_itmplType(QVariant _value, const QByteArray& _paramName)> fromVariant;
    std::function<QStringList()> optionsLambda;

    friend class Private::intfCacheConnector;
};

}
#endif // QHTTP_PRIVATE_TMPLAPIARG_HPP
