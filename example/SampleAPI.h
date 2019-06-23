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

#ifndef SAMPLEAPI_H
#define SAMPLEAPI_H

#include "QHttp/intfRESTAPIHolder.h"

namespace Sample1 {
class SampleAPI : public QHttp::intfRESTAPIHolder
{
    Q_OBJECT
public:
    void init();

private slots:
    int apiGETSampleData();
    QHttp::COOKIES_t apiGETSampleDataWithCookie(QHttp::COOKIES_t _COOKIES);
    QHttp::HEADERS_t apiGETSampleDataWithHeaders(QHttp::HEADERS_t _HEADERS);
    QString apiGETSampleDataReturningJWT();
    QHttp::JWT_t apiGETSampleDataWithJWT(QHttp::JWT_t _JWT);
    int apiPUTSampleData(quint64 _id, const QString& _info = "defaultValue");
    int apiDELETESampleData(quint64 _id = 5);
    QVariantList apiUPDATESampleData(quint64 _id, const QString &_info = "dfdsf");


    QHttp::stuTable apiTranslate(const QString& _text, bool _info = ",");
    QHttp::stuTable apiSampleList(const QVariantList& _list);
    QString apiWSSample(const QString _value);

private:
    SampleAPI();
    TARGOMAN_DEFINE_SINGLETON_MODULE(SampleAPI);
};

}

#endif // SAMPLEAPI_H
