/***************************************************************************
                          modifyconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H
#define MODIFYCONSTRAINTTEACHERMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H

#include "ui_modifyconstraintteachermaxtwoactivitytagsperrealdayfromn1n2n3form_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form : public QDialog, Ui::ModifyConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3* _ctr;

	ModifyConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form(QWidget* parent, ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3* ctr);
	~ModifyConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3Form();

public slots:
	void ok();
	void cancel();
};

#endif
