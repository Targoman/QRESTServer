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

#ifndef QHTTP_INTFRESTAPI_H
#define QHTTP_INTFRESTAPI_H

#include "libTargomanCommon/exTargomanBase.h"
#include "libTargomanCommon/clsCountAndSpeed.h"
#include "libTargomanCommon/Configuration/intfConfigurableModule.hpp"
#include "QHttp/HTTPExceptions.h"

namespace QHttp {
/**********************************************************************/
class intfAPIObject{
public:
    virtual void invokeMethod(const QVariantList& _arguments, QGenericReturnArgument _returnArg) const = 0;
};

class intfAPIArgManipulator{
public:
    intfAPIArgManipulator(const QString& _realTypeName);

    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage) = 0;
    virtual QVariant invokeMethod(const intfAPIObject* _apiObject, const QVariantList& _arguments) = 0;
    virtual void cleanup (void* _argStorage) = 0;

    QString     PrettyTypeName;
    char*       RealTypeName;
};

template<typename _itmplType>
class tmplAPIArg : public intfAPIArgManipulator{
public:
    tmplAPIArg(const QString& _prettyName, std::function<QVariant(_itmplType _value)> _toVariant);
    virtual QGenericArgument makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage);
    virtual QVariant invokeMethod(const intfAPIObject *_apiObject, const QVariantList& _arguments);
    virtual void cleanup (void* _argStorage);

private:
    std::function<QVariant(_itmplType _value)> toVariant;
};

#define QHTTP_REGISTER_METATYPE(_type, _lambdaToVariant) \
    qRegisterMetaType<_type>(); \
    gUserDefinedTypesInfo.insert( \
        QMetaType::type(TARGOMAN_M2STR(_type)) - 1025, \
                        new tmplAPIArg<_type>(TARGOMAN_M2STR(_type), _lambdaToVariant))


/**********************************************************************/
class intfRESTSpecialArgumntType{
public:
    virtual QVariant toVariant() = 0;
};

/**
 * @brief The stuStatistics struct
 */
struct stuStatistics {
    Targoman::Common::clsCountAndSpeed Connections;
    Targoman::Common::clsCountAndSpeed Errors;
    Targoman::Common::clsCountAndSpeed Blocked;
    Targoman::Common::clsCountAndSpeed Success;

    QHash<QString, Targoman::Common::clsCountAndSpeed> APICallsStats;
    QHash<QString, Targoman::Common::clsCountAndSpeed> APICacheStats;
};

/**
 * @brief The stuTable struct
 */
struct stuTable : public intfRESTSpecialArgumntType{
    qint64 TotalRows;
    QVariantMap Rows;
    stuTable(qint64 _totalRows = -1, const QVariantMap& _rows = QVariantMap()):
        TotalRows(_totalRows),
        Rows(_rows)
    {}
    virtual ~stuTable(){}

    QVariant toVariant(){
        return QVariantMap({{"totalCount", this->TotalRows}, {"rows", this->Rows}});
    }
};

/** @TODO document QT_NO_CAST_FROM_ASCII */

/**
  * @brief CACHEABLE macros are predefined macros in order to mark each API cache TTL. You can add more cache time as you wish while
  *        following cache definition pattern "\d+(S|M|H)" where last character means S: Seconds, M: Minutes, H: Hours and digits must be between 0 to 16384
  */
#ifndef Q_MOC_RUN
#  define CACHEABLE_1S
#  define CACHEABLE_3S
#  define CACHEABLE_5S
#  define CACHEABLE_10S
#  define CACHEABLE_30S
#  define CACHEABLE_1M
#  define CACHEABLE_5M
#  define CACHEABLE_10M
#  define CACHEABLE_1H
#  define CACHEABLE_3H
#  define CACHEABLE_6H
#  define CACHEABLE_12H
#  define CACHEABLE_24H
#  define CACHEABLE_INF
#endif

/**********************************************************************/
/**
 * @brief The intfRESTAPIHolder class is an interface to defines modules which export REST APIs. Such modules must have followint characteristics:
 *   1- Must inherit from @see intfRESTAPIHolder
 *   2- Must include One of the following macros at the end of their class definition:
 *      + TARGOMAN_DEFINE_SINGLETON_MODULE
 *      + TARGOMAN_DEFINE_SINGLETON_SUBMODULE
 */
class intfRESTAPIHolder : public Targoman::Common::Configuration::intfModule{
    Q_OBJECT
public:
    intfRESTAPIHolder(Targoman::Common::Configuration::intfModule *_parent = NULL);
    virtual ~intfRESTAPIHolder(){}

private slots:
    /**
     * @brief apiListAPIs A default slot in all the modules to list APIs registered in that module. This API output will be cached forever
     *        so just the first call for each module will cost some
     * @param _showParams if set to `true` will list API parameters else just API name will be output
     * @param _showTypes if set to `true` will show input types of API parameters and return type else just names will be shown
     * @param _prettifyTypes if set to true a pretty and general form of Qt Types will be printed else will print original QMetaType names
     * @return a list of APIs registered in the module
     */
    CACHEABLE_INF QStringList apiGETListOfAPIs(bool _showParams = true, bool _showTypes = true, bool _prettifyTypes = true);

protected:
    /**
     * @brief exportAPIs will detect and export acceptable functions to API registry.
     * Acceptable functions must have following conditions:
     *  1- must be defined as slots
     *  2- Must use one of the following naming conventions
     *      - Functions working on data: these functions must start with 'api' keyword followed by the HTTP method that can be used:
     *        + GET: to get info about a single entry or list of some entries also is usefull for downloading files. These functions
     *               can be accessed by both GET/POST method
     *        + PUT: to create new entry
     *        + DEL: to delete entry
     *        + UPDATE: to update entry
     *      - complex functions which does not work on single data (e.g. translate, detectLangugae, etc.)  must start with 'api' keyword and continue with their purpose name. These
     *        functions can be accessed by GET/POST method
     *  3- On any error they must throw one of HTTP execptions ()
     * This method must be called in subclasses constructor
     */
    void registerMyRESTAPIs();
};

template<typename _itmplType>
tmplAPIArg<_itmplType>::tmplAPIArg(const QString& _name, std::function<QVariant(_itmplType)> _toVariant) :
    intfAPIArgManipulator(_name),
    toVariant(_toVariant)
{}

template<typename _itmplType>
inline QGenericArgument tmplAPIArg<_itmplType>::makeGenericArgument(const QVariant& _val, const QByteArray& _paramName, void** _argStorage){
    if(!_val.canConvert<_itmplType>())
        throw exHTTPBadRequest("Invalid value specified for parameter: " + _paramName);
    *_argStorage = new _itmplType;
    *((_itmplType*)*_argStorage) = _val.value<_itmplType>();

    return QGenericArgument(this->RealTypeName, *_argStorage);
}

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

template<typename _itmplType>
inline QVariant tmplAPIArg<_itmplType>::invokeMethod(const intfAPIObject* _apiObject, const QVariantList& _arguments){
   _itmplType Result;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
   _apiObject->invokeMethod(_arguments,QReturnArgument<_itmplType >(this->RealTypeName, Result));
#pragma GCC diagnostic pop

   return this->toVariant == nullptr ? QVariant::fromValue(Result) : this->toVariant(Result);
}

template<typename _itmplType>
void tmplAPIArg<_itmplType>::cleanup (void* _argStorage){
    delete ((_itmplType*)_argStorage);
}

}

Q_DECLARE_METATYPE(QHttp::stuTable)
//Q_DECLARE_METATYPE(QHttp::stuStatistics)



#endif // QHTTP_INTFRESTAPI_H
