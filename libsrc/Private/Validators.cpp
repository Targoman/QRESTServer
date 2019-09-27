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

#include <QRegularExpression>

#include "Validators.h"
#include "HTTPExceptions.h"

namespace QHttp {
namespace Private {

#define createErrorString(_message) {if(this->CanThrow) throw exHTTPBadRequest(_message);  else return false;}

#define VALIDATOR_BY_REGEX(_name, _regex, _messageOnRequired, _messageOnError)

/*
bool Validator::email(const QString& _value, bool _required)
{
    static QRegularExpression rxEmail("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$", QRegularExpression::CaseInsensitiveOption);
    if(_required && _value.isEmpty())
       createErrorString("email is required");
    if(rxEmail.match(_value).hasMatch())
        return true;
    createErrorString("Invalid Email Address");
}

bool Validator::md5(const QString& _value, bool _required)
{
    static QRegularExpression rxMd5("^[a-f0-9]{32}$", QRegularExpression::CaseInsensitiveOption);
    if(_required && _value.isEmpty())
       createErrorString("email is required");
    if(rxMd5.match(_value).hasMatch())
        return true;
    createErrorString("Invalid Email Address");
}
*/

Validator::Validator() :
    CanThrow(false)
{;}

ThrowableValidator::ThrowableValidator()
{
    this->CanThrow = true;
}

}
}
