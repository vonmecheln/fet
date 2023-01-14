/***************************************************************************
                          addconstraintteachermaxhoursdailyrealdaysform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERMAXHOURSDAILYREALDAYSFORM_H
#define ADDCONSTRAINTTEACHERMAXHOURSDAILYREALDAYSFORM_H

#include "ui_addconstraintteachermaxhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxHoursDailyRealDaysForm : public QDialog, Ui::AddConstraintTeacherMaxHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxHoursDailyRealDaysForm(QWidget* parent);
	~AddConstraintTeacherMaxHoursDailyRealDaysForm();

	void updateMaxHoursSpinBox();

public slots:
	void addCurrentConstraint();
};

#endif
