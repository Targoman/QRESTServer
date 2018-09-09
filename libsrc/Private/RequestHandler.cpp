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
 * @author S.Mehran M.Ziabary <ziabary@targoman.com>
 */


#include <QJsonObject>
#include <QJsonDocument>
#include "clsRequestHandler.h"
#include "libTargomanCommon/CmdIO.h"
#include "intfRESTAPIHolder.h"
#include "Configs.hpp"

namespace QHttp {
namespace Private {

stuConfigs gConfigs;
stuStatistics gServerStats;

using namespace qhttp::server;
using namespace Targoman::Common;

void RequestHandler::process(const QString& _api, qhttp::server::QHttpRequest *_req, qhttp::server::QHttpResponse *_res) {
    _req->onData([_req, _res](QByteArray _data){
        QByteArray ContentType= _req->headers().value("content-type");
        if(ContentType.isEmpty())
            throw exHTTPBadRequest("No content-type header present");

        switch(ContentType.at(0)){
        case 'a':{
            if(ContentType != "application/json")
                throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());
            _data = _data.trimmed();
            if(_data.startsWith('{') == false || _data.endsWith('}') == false)
                throw exHTTPBadRequest("Invalid JSON Object");
            QJsonParseError Error;
            QJsonDocument JSON = QJsonDocument::fromJson(_data, &Error);
            if(JSON.isNull() || JSON.isObject() == false)
                throw exHTTPBadRequest(QString("Invalid JSON Object: %1").arg(Error.errorString()));
            QJsonObject JSONObject = JSON.object();
            for(auto JSONObjectIter = JSONObject.begin();
                JSONObjectIter != JSONObject.end()
                ++JSONObjectIter)
                _req->addUserDefinedData(JSONObjectIter.key(), JSONObjectIter.value().toString());
            foreach
            break;
        }
        case: 'm':{
            static const char MULTIPART_BOUNDARY_HEADER[] = "multipart/form-data; boundary=";
            static const char MULTIPART_CONTENT_DISPOSITION[] = "Content-Disposition: ";
            static const char MULTIPART_CONTENT_DISPOSITION_FORM[] =        "form-data; name=\"";
            static const char MULTIPART_CONTENT_DISPOSITION_FILE[] =        "file; filename=\"";
            static const char MULTIPART_CONTENT_DISPOSITION_SINGLE_FILE[] = "form-data; name=\"files\"; filename=\"";
            static const char MULTIPART_CONTENT_TYPE_FILES[]       = "Type: multipart/mixed; boundary=";
            static const char MULTIPART_CONTENT_FILETYPE[] = "Content-Type: ";

            if(ContentType.startsWith(MULTIPART_BOUNDARY_HEADER))
                throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());
            QByteArray BaseMarker = ContentType.mid(sizeof(MULTIPART_BOUNDARY_HEADER));
            if(_data.startsWith(BaseMarker) == false)
                throw exHTTPBadRequest(("invalid marker at start of multipart message: " + BaseMarker).constData());
            int NextDataPos = BaseMarker.size();
            QByteArray InnerMarker;
            while(NextDataPos >0){
                /// @note data must start with a Content-Disopsition header followed by its type so check for size
                if(Q_UNLIKELY(_data.size() <= NextDataPos + sizeof(MULTIPART_CONTENT_DISPOSITION) + sizeof(MULTIPART_CONTENT_DISPOSITION_FILE) + 4))
                    throw exHTTPBadRequest("invalid multipart message: no data after marker");
                switch(_data.at(NextDataPos)){
                case '\n':
                case '\r':
                    ++NextDataPos;
                    continue;
                case 'C':
                    switch()
                    _data = _data.mid(NextDataPos + sizeof(MULTIPART_CONTENT_DISPOSITION));
                    if(_data.startsWith(MULTIPART_CONTENT_DISPOSITION_FORM)){
                        if(InnerMarker.size())
                            throw exHTTPBadRequest("No more form data allowed when iner boundary is set");
                        NextDataPos = _data.indexOf('\"');
                        if(NextDataPos < 0 )
                            throw exHTTPBadRequest("No name for multipart data");
                        QString Name = _data.mid(0, NextDataPos);
                        quint8  HeaderMarker =0;
                        bool DataStarted = false;
                        while(NextDataPos > 0){
                            if(Q_UNLIKELY(_data.size() <= NextDataPos + BaseMarker.size() + 4))
                                throw exHTTPBadRequest("invalid multipart message: no data after header");
                            if(_data.at(NextDataPos) == '\n')
                                ++HeaderMarker;
                            if(HeaderMarker == 2){
                                NextDataPos = _data.indexOf(BaseMarker);
                                if(NextDataPos < 0)
                                    throw exHTTPBadRequest("end of data has not been yet received");
                                break;
                            }
                            ++NextDataPos;
                        }
                    }else if (_data.startsWith(MULTIPART_CONTENT_DISPOSITION_SINGLE_FILE)){
                        if(InnerMarker.size())
                            throw exHTTPBadRequest("No more form data allowed when iner boundary is set");

                    }
                default:
                    throw exHTTPBadRequest(("invalid multipart message: must start with Content-Disposition: " + _data.left(20)).constData());
                }
            }
        }
        default:'
            throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());
        }

        qDebug()<<_req->headers();
        qDebug()<<_data;
        qDebug()<<_req->url().query();
    });
    _req->onEnd([_api, _req, _res](){
        try{
            RequestHandler::findAndCallAPI (_api, _req, _res);
        }catch(exHTTPError &ex){
            RequestHandler::sendError(_res, (qhttp::TStatusCode)ex.code(), ex.what(), ex.code() >= 500);
        }catch(exTargomanBase &ex){
            RequestHandler::sendError(_res, qhttp::ESTATUS_INTERNAL_SERVER_ERROR, ex.what(), true);
        }
    });
}

const qhttp::TStatusCode StatusCodeOnMethod[] = {
    qhttp::ESTATUS_ACCEPTED,           ///< EHTTP_DELETE         =  0,
    qhttp::ESTATUS_OK,                 ///< EHTTP_GET            =  1,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_HEAD           =  2,
    qhttp::ESTATUS_OK,                 ///< EHTTP_POST           =  3,
    qhttp::ESTATUS_CREATED,            ///< EHTTP_PUT            =  4,
    /* pathological */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_CONNECT        =  5,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_OPTIONS        =  6,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_TRACE          =  7,
    /* webdav */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_COPY           =  8,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_LOCK           =  9,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MKCOL          = 10,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MOVE           = 11,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_PROPFIND       = 12,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_PROPPATCH      = 13,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_SEARCH         = 14,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_UNLOCK         = 15,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_BIND           = 16,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_REBIND         = 17,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_UNBIND         = 18,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_ACL            = 19,
    /* subversion */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_REPORT         = 20,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MKACTIVITY     = 21,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_CHECKOUT       = 22,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MERGE          = 23,
    /* upnp */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MSEARCH        = 24,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_NOTIFY         = 25,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_SUBSCRIBE      = 26,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_UNSUBSCRIBE    = 27,
    /* RFC-5789 */
    qhttp::ESTATUS_ACCEPTED,           ///< EHTTP_PATCH          = 28,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_PURGE          = 29,
    /* CalDAV */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_MKCALENDAR     = 30,
    /* RFC-2068, section 19.6.1.2 */
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_LINK           = 31,
    qhttp::ESTATUS_EXPECTATION_FAILED, ///< EHTTP_UNLINK         = 32,
};

void RequestHandler::findAndCallAPI(const QString& _api, QHttpRequest *_req, QHttpResponse *_res)
{
    clsAPIObject* APIObject = RESTAPIRegistry::getAPIObject(_req->methodString(), _api);

    if(!APIObject){
        gServerStats.Errors.inc();
        return RequestHandler::sendError(_res,
                                         qhttp::ESTATUS_NOT_FOUND,
                                         "API not found("+_req->methodString()+": "+_api+")",
                                         true);
    }

    QStringList Queries = _req->url().query().split('&', QString::SkipEmptyParts);
    RequestHandler::sendResponse(_res, StatusCodeOnMethod[_req->method()], APIObject->invoke(Queries));

}

void RequestHandler::sendError(QHttpResponse *_res, qhttp::TStatusCode _code, const QString &_message, bool _closeConnection)
{
    QJsonObject ErrorInfo = QJsonObject({
                                            {"code", _code},
                                            {"message", _message}
                                        });
    QByteArray ErrorJson = QJsonDocument(QJsonObject({ {"error", ErrorInfo }})).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);
    _res->setStatusCode(_code);
    if(_closeConnection) _res->addHeader("connection", "close");
    _res->addHeaderValue("content-type", QString("application/json; charset=utf-8"));
    _res->addHeaderValue("content-length", ErrorJson.length());
    _res->end(ErrorJson.constData());
}

void RequestHandler::sendResponse(QHttpResponse *_res, qhttp::TStatusCode _code, QVariant _response)
{
    QByteArray Data = QJsonDocument(QJsonObject({ {"data", QJsonValue::fromVariant(_response) } })).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);

    _res->setStatusCode(_code);
    _res->addHeaderValue("content-length", Data.length());
    _res->addHeaderValue("content-type", QString("application/json; charset=utf-8"));
    _res->end(Data.constData());
}


}
}
