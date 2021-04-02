/***************************************************************************
                          constraintactivitiesoccupymintimeslotsfromselectionform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#ifndef CONSTRAINTACTIVITIESOCCUPYMINTIMESLOTSFROMSELECTIONFORM_H
#define CONSTRAINTACTIVITIESOCCUPYMINTIMESLOTSFROMSELECTIONFORM_H

#include "ui_constraintactivitiesoccupymintimeslotsfromselectionform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm : public QDialog, Ui::ConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm(QWidget* parent);
	~ConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
	
	void help();
};

#endif
