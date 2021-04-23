/***************************************************************************
                          modifyconstraintstudentssetafternoonintervalmaxdaysperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintstudentssetafternoonintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* _ctr;

	ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm(QWidget* parent, ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* ctr);
	~ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
