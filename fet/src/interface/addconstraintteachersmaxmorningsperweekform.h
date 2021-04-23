/***************************************************************************
                          addconstraintteachersmaxmorningsperweekform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERSMAXMORNINGSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERSMAXMORNINGSPERWEEKFORM_H

#include "ui_addconstraintteachersmaxmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxMorningsPerWeekForm : public QDialog, Ui::AddConstraintTeachersMaxMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxMorningsPerWeekForm(QWidget* parent);
	~AddConstraintTeachersMaxMorningsPerWeekForm();

	void updateMaxMorningsSpinBox();
	//void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
