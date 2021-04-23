/***************************************************************************
                          modifyconstraintteachersmaxrealdaysperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXREALDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXREALDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachersmaxrealdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxRealDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMaxRealDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMaxRealDaysPerWeek* _ctr;

	ModifyConstraintTeachersMaxRealDaysPerWeekForm(QWidget* parent, ConstraintTeachersMaxRealDaysPerWeek* ctr);
	~ModifyConstraintTeachersMaxRealDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	//void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
