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
#include <map>
#include <utility>

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

clsRequestHandler::clsRequestHandler(QHttpRequest *_req, QHttpResponse *_res, QObject* _parent) :
    QObject(_parent),
    Request(_req),
    Response(_res)
{}



void clsRequestHandler::process(const QString& _api) {
    this->Request->onData([this](QByteArray _data){
        QByteArray ContentType= this->Request->headers().value("content-type");
        if(ContentType.isEmpty())
            throw exHTTPBadRequest("No content-type header present");
        QByteArray ContentLengthStr = this->Request->headers().value("content-length");
        if(ContentLengthStr.isEmpty())
            throw exHTTPBadRequest("No content-length header present");

        int ContentLength = ContentLengthStr.toLongLong ();
        if(!ContentLength)
            throw exHTTPBadRequest("content-length seems to be zero");

        switch(this->Request->method()){
        case qhttp::EHTTP_POST:
        case qhttp::EHTTP_PUT:
        case qhttp::EHTTP_PATCH:
            break;
        default:
            throw exHTTPBadRequest("Method: "+this->Request->methodString()+" is not supported or does not accept request body");
        }
        static const char APPLICATION_JSON_HEADER[] = "application/json";
        static const char MULTIPART_BOUNDARY_HEADER[] = "multipart/form-data; boundary=";

        switch(ContentType.at(0)){
        case 'a':{
            if(ContentType != APPLICATION_JSON_HEADER)
                throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());

            if(_data.size() == ContentLength){
                this->RemainingData = _data;
            }else if (this->RemainingData.size()){
                this->RemainingData += _data;
                if(this->RemainingData.size() < ContentLength)
                    return;
            }else{
                this->RemainingData = _data;
                return;
            }

            this->RemainingData = this->RemainingData.trimmed();
            if(this->RemainingData.startsWith('{') == false || this->RemainingData.endsWith('}') == false)
                throw exHTTPBadRequest("Invalid JSON Object");
            QJsonParseError Error;
            QJsonDocument JSON = QJsonDocument::fromJson(this->RemainingData, &Error);
            if(JSON.isNull() || JSON.isObject() == false)
                throw exHTTPBadRequest(QString("Invalid JSON Object: %1").arg(Error.errorString()));
            QJsonObject JSONObject = JSON.object();
            for(auto JSONObjectIter = JSONObject.begin();
                JSONObjectIter != JSONObject.end();
                ++JSONObjectIter)
                this->Request->addUserDefinedData(JSONObjectIter.key(), JSONObjectIter.value().toString());

            break;
        }
        case 'm':{
            if(this->MultipartFormDataHandler.isNull()){
                if(ContentType.startsWith(MULTIPART_BOUNDARY_HEADER))
                    throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());

                this->MultipartFormDataHandler.reset(
                            new clsMultipartFormDataRequestHandler(
                                this,
                                ContentType.mid(sizeof(MULTIPART_BOUNDARY_HEADER))
                                ));
            }

            size_t Fed = 0;
            while(!this->MultipartFormDataHandler->stopped() && _data.size() > Fed){
                do {
                    size_t Ret = this->MultipartFormDataHandler->feed(_data.mid(Fed), _data.size() - Fed);
                    Fed += Ret;
                } while (Fed < _data.size() && !this->MultipartFormDataHandler->stopped());
            }

        }
        default:
            throw exHTTPBadRequest(("unsupported Content-Type: " + ContentType).constData());
        }
    });
    this->Request->onEnd([this, _api](){
        try{
            this->findAndCallAPI (_api);
        }catch(exHTTPError &ex){
            this->sendError((qhttp::TStatusCode)ex.code(), ex.what(), ex.code() >= 500);
        }catch(exTargomanBase &ex){
            this->sendError(qhttp::ESTATUS_INTERNAL_SERVER_ERROR, ex.what(), true);
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

void clsRequestHandler::findAndCallAPI(const QString &_api)
{
    clsAPIObject* APIObject = RESTAPIRegistry::getAPIObject(this->Request->methodString(), _api);

    if(!APIObject){
        gServerStats.Errors.inc();
        return this->sendError(qhttp::ESTATUS_NOT_FOUND,
                               "API not found("+this->Request->methodString()+": "+_api+")",
                               true);
    }

    QStringList Queries = this->Request->url().query().split('&', QString::SkipEmptyParts);
    this->sendResponse(StatusCodeOnMethod[this->Request->method()], APIObject->invoke(Queries));

}

void clsRequestHandler::sendError(qhttp::TStatusCode _code, const QString &_message, bool _closeConnection)
{
    QJsonObject ErrorInfo = QJsonObject({
                                            {"code", _code},
                                            {"message", _message}
                                        });
    QByteArray ErrorJson = QJsonDocument(QJsonObject({ {"error", ErrorInfo }})).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);
    this->Response->setStatusCode(_code);
    if(_closeConnection) this->Response->addHeader("connection", "close");
    this->Response->addHeaderValue("content-type", QString("application/json; charset=utf-8"));
    this->Response->addHeaderValue("content-length", ErrorJson.length());
    this->Response->end(ErrorJson.constData());
    this->deleteLater();
}

void clsRequestHandler::sendResponse(qhttp::TStatusCode _code, QVariant _response)
{
    QByteArray Data = QJsonDocument(QJsonObject({ {"data", QJsonValue::fromVariant(_response) } })).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);

    this->Response->setStatusCode(_code);
    this->Response->addHeaderValue("content-length", Data.length());
    this->Response->addHeaderValue("content-type", QString("application/json; charset=utf-8"));
    this->Response->end(Data.constData());
    this->deleteLater();
}

/**************************************************************************/
void clsMultipartFormDataRequestHandler::onMultiPartBegin(const MultipartHeaders &_headers, void *_userData) {
    clsMultipartFormDataRequestHandler *Self = static_cast<clsMultipartFormDataRequestHandler*>(_userData);
    std::string ContentDisposition = _headers["Content-Disposition"];
    if(ContentDisposition.size()){
        const char* pContentDisposition = ContentDisposition.c_str();
        const char* pBufferStart = pContentDisposition;
        enum enuLooking4{
            L4Type,
            L4Field,
            L4Equal,
            L4DQuote,
            L4Value,
        } Looking4 = L4Type;
        char StopChar = ';';
        std::string* pLastFieldValue;

        while(pContentDisposition){
            if(*pContentDisposition == StopChar)
                switch(Looking4){
                case L4Type:
                    if(strncmp(pBufferStart, "form-data", pContentDisposition - pBufferStart))
                        throw exHTTPBadRequest("Just form-data is allowed in multi-part request according to RFC7578");
                    Looking4 = L4Field;
                    break;
                case L4Field:
                    if(strncmp(pBufferStart, "name", pContentDisposition - pBufferStart))
                        pLastFieldValue = &Self->LastItemName;
                    else if(strcmp(pBufferStart, "filename"))
                        pLastFieldValue = &Self->LastFileName;
                    Looking4 = L4Equal;
                    StopChar = '=';
                    break;
                case L4Equal:
                    Looking4 = L4DQuote;
                    StopChar = '"';
                    break;
                case L4DQuote:
                    Looking4 = L4Value;
                    StopChar = '"';
                    pBufferStart = pContentDisposition;
                    break;
                case L4Value:
                    *pLastFieldValue = pBufferStart;
                    pLastFieldValue->erase(pContentDisposition - pBufferStart, std::string::npos);
                }
        }

        if(Self->LastItemName.empty())
            throw exHTTPBadRequest(QString("No name provided for form field: ") + ContentDisposition.c_str());
        if(Self->LastFileName.size()){
            Self->LastTempFile.reset(new QTemporaryFile);
            if(Self->LastTempFile.isNull() || Self->LastTempFile->isOpen() == false)
                throw exHTTPInternalServerError("unable to create temporary file");
            Self->LastTempFile->setAutoRemove(false);
            Self->LastMime = _headers["Content-Type"];
        }
    }else
        throw exHTTPBadRequest("No Content-Disposition header provided");
}

void clsMultipartFormDataRequestHandler::onMultiPartData(const char *_buffer, size_t _size, void *_userData) {
    clsMultipartFormDataRequestHandler *Self = static_cast<clsMultipartFormDataRequestHandler*>(_userData);
    if(Self->LastTempFile.isNull() == false)
        Self->LastWrittenBytes += Self->LastTempFile->write(_buffer, _size);
    else
        Self->pParent->Request->addUserDefinedData(Self->LastItemName.c_str(), QString::fromUtf8(_buffer, _size));
}

void clsMultipartFormDataRequestHandler::onMultiPartEnd(void *_userData) {
    clsMultipartFormDataRequestHandler *Self = static_cast<clsMultipartFormDataRequestHandler*>(_userData);
    if(Self->LastTempFile.isNull() == false){
        if(Self->LastStoredItemName != Self->LastItemName){
            Self->pParent->Request->addUserDefinedData(Self->LastStoredItemName.c_str(), QString("[%1]").arg(Self->UploadedFilesInfo.join(',')));
            Self->LastStoredItemName = Self->LastItemName;
            Self->UploadedFilesInfo.clear();
        }else
            Self->UploadedFilesInfo.append(
                        QString("{\"name\":\"%1\",\"tmpname\":\"%2\",\"size\":\"%3\",\"type\":\"%4\"}").arg(
                            Self->LastFileName.c_str()).arg(
                            Self->LastTempFile->fileName()).arg(
                            Self->LastWrittenBytes).arg(
                            Self->LastMime.c_str()));
    }
    Self->LastFileName.clear();
    Self->LastItemName.clear();
    Self->LastMime.clear();
    Self->LastTempFile.reset();
    Self->LastWrittenBytes = 0;
}

void clsMultipartFormDataRequestHandler::onDataEnd(void *_userData){
    clsMultipartFormDataRequestHandler *Self = static_cast<clsMultipartFormDataRequestHandler*>(_userData);
    if(Self->UploadedFilesInfo.size())
        Self->pParent->Request->addUserDefinedData(Self->LastStoredItemName.c_str(), QString("[%1]").arg(Self->UploadedFilesInfo.join(',')));
}

}
}
