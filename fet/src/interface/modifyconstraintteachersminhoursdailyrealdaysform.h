/***************************************************************************
                          modifyconstraintteachersminhoursdailyrealdaysform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINHOURSDAILYREALDAYSFORM_H
#define MODIFYCONSTRAINTTEACHERSMINHOURSDAILYREALDAYSFORM_H

#include "ui_modifyconstraintteachersminhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinHoursDailyRealDaysForm : public QDialog, Ui::ModifyConstraintTeachersMinHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMinHoursDailyRealDays* _ctr;

	ModifyConstraintTeachersMinHoursDailyRealDaysForm(QWidget* parent, ConstraintTeachersMinHoursDailyRealDays* ctr);
	~ModifyConstraintTeachersMinHoursDailyRealDaysForm();

	void updateMinHoursSpinBox();

public slots:
	void ok();
	void cancel();

	void allowEmptyDaysCheckBoxToggled();
};

#endif
