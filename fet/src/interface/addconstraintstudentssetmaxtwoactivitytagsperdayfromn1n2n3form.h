/***************************************************************************
                          addconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTSTUDENTSSETMAXTWOACTIVITYTAGSPERDAYFROMN1N2N3FORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXTWOACTIVITYTAGSPERDAYFROMN1N2N3FORM_H

#include "ui_addconstraintstudentssetmaxtwoactivitytagsperdayfromn1n2n3form_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Form : public QDialog, Ui::AddConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Form_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Form(QWidget* parent);
	~AddConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3Form();

public slots:
	void addCurrentConstraint();
};

#endif
