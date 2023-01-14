/***************************************************************************
                          addconstraintstudentssetmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTSTUDENTSSETMAXDAYSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXDAYSPERWEEKFORM_H

#include "ui_addconstraintstudentssetmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxDaysPerWeekForm : public QDialog, Ui::AddConstraintStudentsSetMaxDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxDaysPerWeekForm(QWidget* parent);
	~AddConstraintStudentsSetMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateStudentsComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
