/***************************************************************************
                          modifyconstraintteachersactivitytagmaxhoursdailyrealdaysform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSACTIVITYTAGMAXHOURSDAILYREALDAYSFORM_H
#define MODIFYCONSTRAINTTEACHERSACTIVITYTAGMAXHOURSDAILYREALDAYSFORM_H

#include "ui_modifyconstraintteachersactivitytagmaxhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersActivityTagMaxHoursDailyRealDaysForm : public QDialog, Ui::ModifyConstraintTeachersActivityTagMaxHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersActivityTagMaxHoursDailyRealDays* _ctr;

	ModifyConstraintTeachersActivityTagMaxHoursDailyRealDaysForm(QWidget* parent, ConstraintTeachersActivityTagMaxHoursDailyRealDays* ctr);
	~ModifyConstraintTeachersActivityTagMaxHoursDailyRealDaysForm();

	void updateMaxHoursSpinBox();

public slots:
	void ok();
	void cancel();
};

#endif
