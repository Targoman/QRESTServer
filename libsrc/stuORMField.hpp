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

#ifndef QHTTP_STUORMFIELD_HPP
#define QHTTP_STUORMFIELD_HPP

#include "intfAPIArgManipulator.h"
#include "QFieldValidator.h"

namespace QHttp {
class intfRESTAPIHolder;
namespace Private {
class RESTAPIRegistry;
}

#define T(_type) #_type

struct stuORMField{
    QString         Name;
    int             ParameterType;
    QString         ParamTypeName;
    QFieldValidator ExtraValidator;
    bool            IsSortable;
    bool            IsFilterable;
    bool            IsReadOnly;
    bool            IsSelfIdentifier;
    bool            IsVirtual;
    qint8           PKIndex;
    QString         RenameAs;

    stuORMField();
    stuORMField(const QString& _name,
                const QString& _type,
                const QFieldValidator& _extraValidator = QFV.allwaysValid(),
                bool _isReadOnly = false,
                bool _isSortable = true,
                bool _isFilterable = true,
                bool _isSelfIdentifier = false,
                bool _isVirtual = false,
                bool _isPrimaryKey = false,
                const QString& _renameAs = {});
    void registerTypeIfNotRegisterd(QHttp::intfRESTAPIHolder* _module);
    void validate(const QVariant _value) const;
};
///                         RO   Sort  Filter Self  Virt   PK
#define ORM_PRIMARY_KEY     true, true, true, true, false, true
#define ORM_SELF_REAL       true, true, true, true, false
#define ORM_SELF_VIRTUAL    true, true, true, true, true

}
#endif // QHTTP_STUORMFIELD_HPP
