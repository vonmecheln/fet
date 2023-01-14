/***************************************************************************
                          addconstraintteachermaxmorningsperweekform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERMAXMORNINGSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERMAXMORNINGSPERWEEKFORM_H

#include "ui_addconstraintteachermaxmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxMorningsPerWeekForm : public QDialog, Ui::AddConstraintTeacherMaxMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxMorningsPerWeekForm(QWidget* parent);
	~AddConstraintTeacherMaxMorningsPerWeekForm();

	void updateMaxMorningsSpinBox();
	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
