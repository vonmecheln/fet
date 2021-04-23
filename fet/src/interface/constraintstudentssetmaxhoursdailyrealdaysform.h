/***************************************************************************
                          constraintstudentssetmaxhoursdailyrealdaysform.h  -  description
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

#ifndef CONSTRAINTSTUDENTSSETMAXHOURSDAILYREALDAYSFORM_H
#define CONSTRAINTSTUDENTSSETMAXHOURSDAILYREALDAYSFORM_H

#include "ui_constraintstudentssetmaxhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintStudentsSetMaxHoursDailyRealDaysForm : public QDialog, Ui::ConstraintStudentsSetMaxHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintStudentsSetMaxHoursDailyRealDaysForm(QWidget* parent);
	~ConstraintStudentsSetMaxHoursDailyRealDaysForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
