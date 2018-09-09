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

clsRequestHandler::clsRequestHandler(QObject* _parent) :
    QObject(_parent),
    LastMarkerPos(0),
    LastRemainingMarkerSize(0)
{}

void clsRequestHandler::process(const QString& _api, qhttp::server::QHttpRequest *_req, qhttp::server::QHttpResponse *_res) {
    _req->onData([_req, _res](QByteArray _data){
        QByteArray ContentType= _req->headers().value("content-type");
        if(ContentType.isEmpty())
            throw exHTTPBadRequest("No content-type header present");
        QByteArray ContentLengthStr = _req->headers().value("content-length");
        if(ContentLengthStr.isEmpty())
            throw exHTTPBadRequest("No content-length header present");

        quint64 ContentLength = ContentLengthStr.toULongLong ();
        if(!ContentLength)
            throw exHTTPBadRequest("content-length seems to be zero");

        switch(_req->method()){
        case qhttp::EHTTP_POST:
        case qhttp::EHTTP_PUT:
        case qhttp::EHTTP_PATCH:
            break;
        default:
            throw exHTTPBadRequest("Method: "+_req->methodString()+" is not supported or does not accept request body");
        }

        switch(ContentType.at(0)){
        case 'a':{
            if(ContentType != "application/json")
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
            if(this->LastMarker.isEmpty())
                this->LastMarker = BaseMarker;

            static auto finishFile = [this, _data](const QByteArray& _toWriteData, int _size){
                if(_size > 0) this->LastStoringFile->write(_toWriteData.constData(), _size);
                this->LastStoringFile.reset();
                this->LastRemainingMarkerSize = 0;
            };

            static auto storeDataContinuationToFile = [this, _data, finishFile](){
                this->LastMarkerPos = _data.indexOf(this->LastMarker);
                if(this->LastMarkerPos < 0){
                    QByteArray CheckingMarker = this->LastMarker;
                    while(CheckingMarker.size() > 1){
                        CheckingMarker.truncate(CheckingMarker.size() - 2);
                        if(_data.endsWith(CheckingMarker)){
                            this->LastStoringFile->write(_data.constData(), _data.size() - CheckingMarker.size());
                            this->RemainingData = _data.mid(_data.size() - CheckingMarker.size());
                            this->LastRemainingMarkerSize = this->LastMarker.size() - CheckingMarker.size();
                            return true;
                        }
                    }
                    this->LastStoringFile->write(_data);
                    this->LastRemainingMarkerSize = 0;
                }else{
                    finishFile(_data, this->LastMarkerPos);
                }
                return false;
            };

            static auto processCompletedData = [this, BaseMarker](const QByteArray& _data){
                this->LastMarkerPos = this->LastMarker.size();
                while(this->LastMarkerPos >0){
                    /// @note data must start with a Content-Disopsition header followed by its type so check for size
                    if(Q_UNLIKELY(_data.size() <= this->LastMarkerPos + sizeof(MULTIPART_CONTENT_DISPOSITION) + sizeof(MULTIPART_CONTENT_DISPOSITION_FILE) + 4))
                        throw exHTTPBadRequest("invalid multipart message: no data after marker");
                    switch(_data.at(this->LastMarkerPos)){
                    case '\n':
                    case '\r':
                        ++this->LastMarkerPos;
                        continue;
                    case 'C':
                        _data = _data.mid(this->LastMarkerPos + sizeof(MULTIPART_CONTENT_DISPOSITION));
                        if (_data.startsWith(MULTIPART_CONTENT_DISPOSITION_SINGLE_FILE)){
                            if(this->LastMarker != BaseMarker)
                                throw exHTTPBadRequest("No more form data allowed when inner boundary is set");
                            this->LastMarkerPos = _data.indexOf('\"');
                            if(this->LastMarkerPos < 0 )
                                throw exHTTPBadRequest("No name for file");

                            QString Name = _data.mid(0, this->LastMarkerPos).constData();
                            QString Mime;
                            quint8  HeaderMarker = 0;
                            while(this->LastMarkerPos > 0){
                                if(Q_UNLIKELY(_data.size() <= this->LastMarkerPos + BaseMarker.size() + 4))
                                    throw exHTTPBadRequest("invalid multipart message: no data after header");
                                if(_data.at(this->LastMarkerPos) == '\n')
                                    ++HeaderMarker;
                                if(HeaderMarker == 2){
                                    this->LastStoringFile.reset(new QTemporaryFile);
                                    if(this->LastStoringFile.isNull() || this->LastStoringFile->open() == false)
                                        throw exHTTPInternalServerError("Seems that temp directory is full");
                                    this->LastStoringFile->setAutoRemove(false);
                                    storeDataContinuationToFile();
                                    QString FileJson = QString("[{\"name\":\"%1\",\"tmp-name\":\"%2\"").arg(
                                                QJsonValue(Name).toString()).arg(
                                                this->LastStoringFile->fileName()
                                                );
                                    if(Mime.size())
                                        FileJson.append(QString(",\"mime-type\":\"%1\"").arg(QJsonValue(Mime).toString()));
                                    FileJson.append("}]");
                                    _req->addUserDefinedData('files', FileJson);
                                    break;
                                }else if(_data.at(this->LastMarkerPos) == 'C'){
                                    if(_data.startsWith(MULTIPART_CONTENT_FILETYPE))
                                        Mime = _data.mid(0, sizeof(MULTIPART_CONTENT_FILETYPE)).constData();
                                }
                                ++this->LastMarkerPos;
                            }
                        }else if(_data.startsWith(MULTIPART_CONTENT_DISPOSITION_FORM)){
                            if(this->LastMarker != BaseMarker)
                                throw exHTTPBadRequest("No more form data allowed when inner boundary is set");
                            this->LastMarkerPos = _data.indexOf('\"');
                            if(this->LastMarkerPos < 0 )
                                throw exHTTPBadRequest("No name for multipart data");
                            QString Name = _data.mid(0, this->LastMarkerPos).constData();
                            quint8  HeaderMarker =0;
                            while(this->LastMarkerPos > 0){
                                if(Q_UNLIKELY(_data.size() <= this->LastMarkerPos + BaseMarker.size() + 4))
                                    throw exHTTPBadRequest("invalid multipart message: no data after header");
                                if(_data.at(this->LastMarkerPos) == '\n')
                                    ++HeaderMarker;
                                if(HeaderMarker == 2){
                                    this->LastMarkerPos = _data.indexOf(this->LastMarker);
                                    if(this->LastMarkerPos < 0){
                                        if(_data.size() >= ContentLength)
                                            throw exHTTPBadRequest("end of data not found");
                                        this->RemainingData = _data;
                                    }else{
                                        if(Name = "files")
                                        _req->addUserDefinedData(Name, _data.mid(0,this->LastMarker));
                                        continue;
                                    }
                                }
                                ++this->LastMarkerPos;
                            }
                        }else if (_data.startsWith(MULTIPART_CONTENT_DISPOSITION_FILE)){
                            if(this->LastMarker == BaseMarker)
                                throw exHTTPBadRequest("file can not be started without form-data");

                        }
                    default:
                        throw exHTTPBadRequest(("invalid multipart message: must start with Content-Disposition: " + _data.left(20)).constData());
                    }
                }
            };

            if (this->LastStoringFile.isNull() == false && this->LastStoringFile->isOpen())){
                if(this->RemainingData.size()){
                    if(this->RemainingData.size() + _data.size () < this->LastMarker.size() + 2){
                        this->RemainingData += _data;
                        return;
                    }else{
                        if((this->RemainingData + _data.mid(0, this->LastRemainingMarkerSize)).endsWith(this->LastMarker)){
                            finishFile(this->RemainingData.mid(0,this->LastMarker.size() - this->LastRemainingMarkerSize), this->LastMarker.size() - this->LastRemainingMarkerSize);
                            _data = _data.mid(this->LastRemainingMarkerSize);
                            this->RemainingData.clear();
                        }else{
                            this->LastStoringFile->write(this->RemainingData);
                            this->RemainingData.clear();
                            storeDataContinuationToFile();
                            return;
                        }
                    }
                }else
                    storeDataContinuationToFile();
            }else if (this->RemainingData.size()){
                if(this->RemainingData.size() + _data.size()  < this->LastMarker.size() + 2){
                    this->RemainingData += _data;
                    return;
                }else{
                    if((this->RemainingData + _data.mid(0, this->LastRemainingMarkerSize)).endsWith(this->LastMarker)){
                        this->RemainingData += _data.mid(0, this->LastRemainingMarkerSize);
                        processCompletedData(this->RemainingData);
                        this->RemainingData.clear();
                        _data.clear();
                    }else{
                        this->LastMarkerPos = _data.indexOf(this->LastMarker);
                        if(this->LastMarkerPos < 0){
                            QByteArray CheckingMarker = this->LastMarker;
                            while(CheckingMarker.size() > 1){
                                CheckingMarker.truncate(CheckingMarker.size() - 2);
                                if(_data.endsWith(CheckingMarker)){
                                    processCompletedData(this->RemainingData + _data.mid(0, _data.size() - CheckingMarker.size() + this->LastMarker.size()));
                                    this->RemainingData = _data.mid(_data.size() - CheckingMarker.size());
                                    this->LastRemainingMarkerSize = this->LastMarker.size() - CheckingMarker.size();
                                    return;
                                }
                            }
                            this->RemainingData += _data;
                            this->LastRemainingMarkerSize = 0;
                        }else
                            _data = processCompletedData(_data.mid(0,this->LastMarkerPos + this->LastMarker.size()))
                    }
                }
            }else if(_data.size() < BaseMarker.size()){
                this->RemainingData = _data;
                return;
            }

            if(_data.startsWith(BaseMarker) == false)
                throw exHTTPBadRequest(("invalid marker at start of multipart message: " + BaseMarker).constData());

            if(_data.size() && this->LastMarkerPos >= 0)
                processCompletedData(_data.mid(this->LastMarkerPos));
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
            this->findAndCallAPI (_api, _req, _res);
        }catch(exHTTPError &ex){
            this->sendError(_res, (qhttp::TStatusCode)ex.code(), ex.what(), ex.code() >= 500);
        }catch(exTargomanBase &ex){
            this->sendError(_res, qhttp::ESTATUS_INTERNAL_SERVER_ERROR, ex.what(), true);
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

void clsRequestHandler::findAndCallAPI(const QString& _api, QHttpRequest *_req, QHttpResponse *_res)
{
    clsAPIObject* APIObject = RESTAPIRegistry::getAPIObject(_req->methodString(), _api);

    if(!APIObject){
        gServerStats.Errors.inc();
        return this->sendError(_res,
                               qhttp::ESTATUS_NOT_FOUND,
                               "API not found("+_req->methodString()+": "+_api+")",
                               true);
    }

    QStringList Queries = _req->url().query().split('&', QString::SkipEmptyParts);
    this->sendResponse(_res, StatusCodeOnMethod[_req->method()], APIObject->invoke(Queries));

}

void clsRequestHandler::sendError(QHttpResponse *_res, qhttp::TStatusCode _code, const QString &_message, bool _closeConnection)
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
    this->deleteLater();
}

void clsRequestHandler::sendResponse(QHttpResponse *_res, qhttp::TStatusCode _code, QVariant _response)
{
    QByteArray Data = QJsonDocument(QJsonObject({ {"data", QJsonValue::fromVariant(_response) } })).toJson(gConfigs.Public.IndentedJson ? QJsonDocument::Indented : QJsonDocument::Compact);

    _res->setStatusCode(_code);
    _res->addHeaderValue("content-length", Data.length());
    _res->addHeaderValue("content-type", QString("application/json; charset=utf-8"));
    _res->end(Data.constData());
    this->deleteLater();
}


}
}
