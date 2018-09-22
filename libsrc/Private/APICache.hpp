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

#ifndef QHTTP_PRIVATE_APIRESULTCACHE_H
#define QHTTP_PRIVATE_APIRESULTCACHE_H

#include <QObject>
#include <QTime>
#include <QString>
#include <QVariant>
#include <QHash>

#include "Private/Configs.hpp"

namespace QHttp {
namespace Private {

struct stuCacheValue{
    QTime InsertionTime;
    QVariant Value;
    qint32   TTL;

    stuCacheValue(){}
    virtual ~stuCacheValue(){}
    stuCacheValue(const QVariant& _value, qint32 _ttl):InsertionTime(QTime::currentTime()),Value(_value), TTL(_ttl){}
    stuCacheValue(const stuCacheValue& _other):InsertionTime(_other.InsertionTime),Value(_other.Value), TTL(_other.TTL){}
};
typedef QHash<QString, stuCacheValue> Cache_t;

class InternalCache
{
public:
    static void setValue(const QString& _key, const QVariant& _value, qint32 _ttl){
        QMutexLocker Locker(&InternalCache::Lock);
        if(InternalCache::Cache.size() < (int)gConfigs.Public.MaxCachedItems)
           InternalCache::Cache.insert(_key, stuCacheValue(_value, _ttl));
    }
    static QVariant storedValue(const QString& _key){
        auto StoredValue = InternalCache::Cache.find(_key);
        if(StoredValue == InternalCache::Cache.end())
            return QVariant();
        if(StoredValue->InsertionTime.secsTo(QTime::currentTime()) > StoredValue->TTL)
            return QVariant();
        return StoredValue->Value;
    }

public:
    static Cache_t Cache;
    static QMutex  Lock;

    friend class clsUpdateAndPruneThread;
};

class CentralCache
{
public:
    static void setValue(const QString& _key, const QVariant& _value, qint32 _ttl){
        QMutexLocker Locker(&InternalCache::Lock);
        if(InternalCache::Cache.size() < (int)gConfigs.Public.MaxCachedItems)
           InternalCache::Cache.insert(_key, stuCacheValue(_value, _ttl));
    }
    static QVariant storedValue(const QString& _key){
        auto StoredValue = InternalCache::Cache.find(_key);
        if(StoredValue == InternalCache::Cache.end())
            return QVariant();
        if(StoredValue->InsertionTime.secsTo(QTime::currentTime()) > StoredValue->TTL)
            return QVariant();
        return StoredValue->Value;
    }

private:
    static Cache_t Cache;
    static QMutex  Lock;

    friend class clsUpdateAndPruneThread;
};

}
}
#endif // CLSAPIRESULTCACHE_H
