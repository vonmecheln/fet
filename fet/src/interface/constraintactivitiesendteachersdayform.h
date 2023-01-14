/***************************************************************************
                          constraintactivitiesendteachersdayform.h  -  description
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

#ifndef CONSTRAINTACTIVITIESENDTEACHERSDAYFORM_H
#define CONSTRAINTACTIVITIESENDTEACHERSDAYFORM_H

#include "ui_constraintactivitiesendteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesEndTeachersDayForm : public QDialog, Ui::ConstraintActivitiesEndTeachersDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesEndTeachersDayForm(QWidget* parent);
	~ConstraintActivitiesEndTeachersDayForm();

	void refreshConstraintsListWidget();

	bool filterOk(TimeConstraint* ctr);
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();
	
};

#endif
