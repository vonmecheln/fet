/***************************************************************************
                          addconstraintstudentssetafternoonintervalmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSSETAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSSETAFTERNOONINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintstudentssetafternoonintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateStudentsComboBox();
	void updateStartHoursComboBox();
	void updateEndHoursComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
