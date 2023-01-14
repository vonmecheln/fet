/***************************************************************************
                          constraintteachersmaxgapspermorningandafternoonform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#ifndef CONSTRAINTTEACHERSMAXGAPSPERMORNINGANDAFTERNOONFORM_H
#define CONSTRAINTTEACHERSMAXGAPSPERMORNINGANDAFTERNOONFORM_H

#include "ui_constraintteachersmaxgapspermorningandafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintTeachersMaxGapsPerMorningAndAfternoonForm : public QDialog, Ui::ConstraintTeachersMaxGapsPerMorningAndAfternoonForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintTeachersMaxGapsPerMorningAndAfternoonForm(QWidget* parent);
	~ConstraintTeachersMaxGapsPerMorningAndAfternoonForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
