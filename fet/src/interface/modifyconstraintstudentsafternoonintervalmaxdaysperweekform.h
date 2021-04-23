/***************************************************************************
                          modifyconstraintstudentsafternoonintervalmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintstudentsafternoonintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsAfternoonIntervalMaxDaysPerWeek* _ctr;

	ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm(QWidget* parent, ConstraintStudentsAfternoonIntervalMaxDaysPerWeek* ctr);
	~ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();

public slots:
	void ok();
	void cancel();
};

#endif
