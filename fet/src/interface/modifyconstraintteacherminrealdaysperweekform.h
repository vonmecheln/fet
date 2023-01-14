/***************************************************************************
                          modifyconstraintteacherminrealdaysperweekform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERMINREALDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERMINREALDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteacherminrealdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMinRealDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeacherMinRealDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeacherMinRealDaysPerWeek* _ctr;

	ModifyConstraintTeacherMinRealDaysPerWeekForm(QWidget* parent, ConstraintTeacherMinRealDaysPerWeek* ctr);
	~ModifyConstraintTeacherMinRealDaysPerWeekForm();

	void updateMinDaysSpinBox();
	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
