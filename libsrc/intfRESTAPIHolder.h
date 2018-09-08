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
using fnDeserializer_t = std::function<void(void* _storage, QVariant _data)>;

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
struct stuTable{
    qint64 TotalCount;
    QVariantMap Items;
};

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

}

Q_DECLARE_METATYPE(QHttp::stuTable)
Q_DECLARE_METATYPE(QHttp::stuStatistics)

#endif // QHTTP_INTFRESTAPI_H
