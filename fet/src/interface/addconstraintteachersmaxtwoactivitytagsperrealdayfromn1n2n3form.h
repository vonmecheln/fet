/***************************************************************************
                          addconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTTEACHERSMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H
#define ADDCONSTRAINTTEACHERSMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H

#include "ui_addconstraintteachersmaxtwoactivitytagsperrealdayfromn1n2n3form_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Form : public QDialog, Ui::AddConstraintTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Form_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Form(QWidget* parent);
	~AddConstraintTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3Form();

public slots:
	void addCurrentConstraint();
};

#endif
