/***************************************************************************
                          modifyconstraintteachersminrealdaysperweekform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINREALDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMINREALDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachersminrealdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinRealDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMinRealDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMinRealDaysPerWeek* _ctr;

	ModifyConstraintTeachersMinRealDaysPerWeekForm(QWidget* parent, ConstraintTeachersMinRealDaysPerWeek* ctr);
	~ModifyConstraintTeachersMinRealDaysPerWeekForm();

	void updateMinDaysSpinBox();
	//void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
