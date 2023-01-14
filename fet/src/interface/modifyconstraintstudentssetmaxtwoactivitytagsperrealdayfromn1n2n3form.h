/***************************************************************************
                          modifyconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H

#include "ui_modifyconstraintstudentssetmaxtwoactivitytagsperrealdayfromn1n2n3form_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Form : public QDialog, Ui::ModifyConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Form_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3* _ctr;

	ModifyConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Form(QWidget* parent, ConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3* ctr);
	~ModifyConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3Form();

public slots:
	void ok();
	void cancel();
};

#endif
