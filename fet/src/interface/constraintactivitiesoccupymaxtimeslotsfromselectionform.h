/***************************************************************************
                          constraintactivitiesoccupymaxtimeslotsfromselectionform.h  -  description
                             -------------------
    begin                : Sept 26, 2011
    copyright            : (C) 2011 by Liviu Lalescu
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

#ifndef CONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H
#define CONSTRAINTACTIVITIESOCCUPYMAXTIMESLOTSFROMSELECTIONFORM_H

#include "ui_constraintactivitiesoccupymaxtimeslotsfromselectionform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm : public QDialog, Ui::ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm(QWidget* parent);
	~ConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm();

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
