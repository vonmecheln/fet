/***************************************************************************
                          modifyconstraintteachersmaxmorningsperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXMORNINGSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXMORNINGSPERWEEKFORM_H

#include "ui_modifyconstraintteachersmaxmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxMorningsPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMaxMorningsPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMaxMorningsPerWeek* _ctr;

	ModifyConstraintTeachersMaxMorningsPerWeekForm(QWidget* parent, ConstraintTeachersMaxMorningsPerWeek* ctr);
	~ModifyConstraintTeachersMaxMorningsPerWeekForm();

	void updateMaxMorningsSpinBox();
	//void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
