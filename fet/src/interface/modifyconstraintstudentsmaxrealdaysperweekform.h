/***************************************************************************
                          modifyconstraintstudentsmaxrealdaysperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXREALDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXREALDAYSPERWEEKFORM_H

#include "ui_modifyconstraintstudentsmaxrealdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxRealDaysPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsMaxRealDaysPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxRealDaysPerWeek* _ctr;

	ModifyConstraintStudentsMaxRealDaysPerWeekForm(QWidget* parent, ConstraintStudentsMaxRealDaysPerWeek* ctr);
	~ModifyConstraintStudentsMaxRealDaysPerWeekForm();

	void updateMaxDaysSpinBox();

public slots:
	void ok();
	void cancel();
};

#endif
