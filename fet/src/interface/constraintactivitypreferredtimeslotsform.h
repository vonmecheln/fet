/***************************************************************************
                          constraintactivitypreferredtimeslotsform.h  -  description
                             -------------------
    begin                : Feb 11, 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#ifndef CONSTRAINTACTIVITYPREFERREDTIMESLOTSFORM_H
#define CONSTRAINTACTIVITYPREFERREDTIMESLOTSFORM_H

#include "ui_constraintactivitypreferredtimeslotsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintActivityPreferredTimeSlotsForm : public QDialog, Ui::ConstraintActivityPreferredTimeSlotsForm_template  {
	Q_OBJECT

public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivityPreferredTimeSlotsForm(QWidget* parent);
	~ConstraintActivityPreferredTimeSlotsForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
