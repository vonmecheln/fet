/***************************************************************************
                          modifyconstraintstudentsmaxhoursdailyrealdaysform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYREALDAYSFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYREALDAYSFORM_H

#include "ui_modifyconstraintstudentsmaxhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxHoursDailyRealDaysForm : public QDialog, Ui::ModifyConstraintStudentsMaxHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxHoursDailyRealDays* _ctr;

	ModifyConstraintStudentsMaxHoursDailyRealDaysForm(QWidget* parent, ConstraintStudentsMaxHoursDailyRealDays* ctr);
	~ModifyConstraintStudentsMaxHoursDailyRealDaysForm();

public slots:
	void ok();
	void cancel();
};

#endif
