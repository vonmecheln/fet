/***************************************************************************
                          addconstraintstudentssetmorningintervalmaxdaysperweekform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETMORNINGINTERVALMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSSETMORNINGINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintstudentssetmorningintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateStudentsComboBox();
	void updateStartHoursComboBox();
	void updateEndHoursComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
