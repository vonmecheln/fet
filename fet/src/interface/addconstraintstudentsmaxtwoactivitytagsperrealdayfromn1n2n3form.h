/***************************************************************************
                          addconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H
#define ADDCONSTRAINTSTUDENTSMAXTWOACTIVITYTAGSPERREALDAYFROMN1N2N3FORM_H

#include "ui_addconstraintstudentsmaxtwoactivitytagsperrealdayfromn1n2n3form_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Form : public QDialog, Ui::AddConstraintStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Form_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Form(QWidget* parent);
	~AddConstraintStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3Form();

public slots:
	void addCurrentConstraint();
};

#endif
