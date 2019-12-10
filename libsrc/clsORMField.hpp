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

#ifndef QHTTP_clsORMField_HPP
#define QHTTP_clsORMField_HPP

#include "intfAPIArgManipulator.h"
#include "QFieldValidator.h"

namespace QHttp {
class intfRESTAPIHolder;
namespace Private {
class RESTAPIRegistry;
}

#define S(_type) #_type

class clsORMFieldData : public QSharedData{
public:
    clsORMFieldData();
    clsORMFieldData(const QString& _name,
                    const QString& _type,
                    const QFieldValidator& _extraValidator,
                    bool _isReadOnly,
                    bool _isSortable,
                    bool _isFilterable,
                    bool _isSelfIdentifier,
                    bool _isVirtual,
                    bool _isPrimaryKey,
                    const QString& _renameAs);
    clsORMFieldData(const clsORMFieldData& _o);
    ~clsORMFieldData() { }

public:
    QString         Name;
    QMetaType::Type ParameterType;
    QString         ParamTypeName;
    QFieldValidator ExtraValidator;
    bool            IsSortable;
    bool            IsFilterable;
    bool            IsReadOnly;
    bool            IsSelfIdentifier;
    bool            IsVirtual;
    bool            IsPrimaryKey;
    QString         RenameAs;
};

class clsORMField{
public:
    clsORMField();
    clsORMField(const clsORMField& _other, const QString& _newName = QString());
    clsORMField(const QString& _name,
                const QString& _type,
                const QFieldValidator& _extraValidator = QFV.allwaysValid(),
                bool _isReadOnly = false,
                bool _isSortable = true,
                bool _isFilterable = true,
                bool _isSelfIdentifier = false,
                bool _isVirtual = false,
                bool _isPrimaryKey = false,
                const QString& _renameAs = {});
    ~clsORMField();
    void registerTypeIfNotRegisterd(QHttp::intfRESTAPIHolder* _module);
    void updateTypeID(QMetaType::Type _type);
    void validate(const QVariant _value) const;
    inline QString         name() const {return this->Data->Name;}
    inline int             parameterType() const {return this->Data->ParameterType;}
    inline QString         paramTypeName() const {return this->Data->ParamTypeName;}
    inline QFieldValidator extraValidator() const {return this->Data->ExtraValidator;}
    inline bool            isSortable() const {return this->Data->IsSortable;}
    inline bool            isFilterable() const {return this->Data->IsFilterable;}
    inline bool            isReadOnly() const {return this->Data->IsReadOnly;}
    inline bool            isSelfIdentifier() const {return this->Data->IsSelfIdentifier;}
    inline bool            isVirtual() const {return this->Data->IsVirtual;}
    inline bool            isPrimaryKey() const {return this->Data->IsPrimaryKey;}
    inline QString         renameAs() const {return this->Data->RenameAs;}

private:
    QExplicitlySharedDataPointer<clsORMFieldData> Data;
};


///                         RO   Sort  Filter Self  Virt   PK
#define ORM_PRIMARY_KEY     true, true, true, false, false, true
#define ORM_SELF_REAL       true, true, true, true, false
#define ORM_SELF_VIRTUAL    true, true, true, true, true

}
#endif // QHTTP_clsORMField_HPP
