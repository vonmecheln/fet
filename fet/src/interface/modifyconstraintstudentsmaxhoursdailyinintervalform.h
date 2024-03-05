/***************************************************************************
                          modifyconstraintstudentsmaxhoursdailyinintervalform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYININTERVALFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXHOURSDAILYININTERVALFORM_H

#include "ui_modifyconstraintstudentsmaxhoursdailyinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxHoursDailyInIntervalForm : public QDialog, Ui::ModifyConstraintStudentsMaxHoursDailyInIntervalForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxHoursDailyInInterval* _ctr;

	ModifyConstraintStudentsMaxHoursDailyInIntervalForm(QWidget* parent, ConstraintStudentsMaxHoursDailyInInterval* ctr);
	~ModifyConstraintStudentsMaxHoursDailyInIntervalForm();

public slots:
	void ok();
	void cancel();
};

#endif
