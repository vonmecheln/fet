/***************************************************************************
                          addconstraintstudentsmorningintervalmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTSTUDENTSMORNINGINTERVALMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSMORNINGINTERVALMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintstudentsmorningintervalmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMorningIntervalMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintStudentsMorningIntervalMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMorningIntervalMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintStudentsMorningIntervalMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateStartHoursComboBox();
	void updateEndHoursComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
