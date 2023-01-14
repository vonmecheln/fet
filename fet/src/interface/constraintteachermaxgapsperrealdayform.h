/***************************************************************************
                          constraintteachermaxgapsperrealdayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Liviu Lalescu
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

#ifndef CONSTRAINTTEACHERMAXGAPSPERREALDAYFORM_H
#define CONSTRAINTTEACHERMAXGAPSPERREALDAYFORM_H

#include "ui_constraintteachermaxgapsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintTeacherMaxGapsPerRealDayForm : public QDialog, Ui::ConstraintTeacherMaxGapsPerRealDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintTeacherMaxGapsPerRealDayForm(QWidget* parent);
	~ConstraintTeacherMaxGapsPerRealDayForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void help();

	void filterChanged();
};

#endif
