/***************************************************************************
                          constraintactivitybeginsstudentsdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITYBEGINSSTUDENTSDAYFORM_H
#define CONSTRAINTACTIVITYBEGINSSTUDENTSDAYFORM_H

#include "ui_constraintactivitybeginsstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivityBeginsStudentsDayForm : public QDialog, Ui::ConstraintActivityBeginsStudentsDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivityBeginsStudentsDayForm(QWidget* parent);
	~ConstraintActivityBeginsStudentsDayForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
