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

#ifndef QHTTP_PRIVATE_NUMERICTYPES_HPP
#define QHTTP_PRIVATE_NUMERICTYPES_HPP

#include "HTTPExceptions.h"
#include "tmplAPIArg.h"

namespace QHttp {

#define QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(_numericType, _convertor) \
template<> inline QGenericArgument tmplAPIArg<_numericType, true>::makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage){ \
    bool Result; *_argStorage = new _numericType; *(reinterpret_cast<_numericType*>(*_argStorage)) = static_cast<_numericType>(_val._convertor(&Result)); \
    if(!Result) throw exHTTPBadRequest("Invalid value specified for parameter: " + _paramName); \
    return QGenericArgument(this->RealTypeName, *_argStorage); \
}

QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint8,  toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint16, toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint32, toUInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(quint64, toULongLong)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint8,   toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint16,  toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint32,  toInt)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qint64,  toLongLong)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(qreal,   toDouble)
QHTTP_SPECIAL_MAKE_GENERIC_ON_NUMERIC_TYPE(float,   toFloat)

}
#endif // QHTTP_PRIVATE_NUMERICTYPES_HPP
