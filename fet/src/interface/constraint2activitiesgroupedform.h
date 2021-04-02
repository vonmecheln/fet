/***************************************************************************
                          constraint2activitiesgroupedform.h  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTRAINT2ACTIVITIESGROUPEDFORM_H
#define CONSTRAINT2ACTIVITIESGROUPEDFORM_H

#include "constraint2activitiesgroupedform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class Constraint2ActivitiesGroupedForm : public Constraint2ActivitiesGroupedForm_template  {
public:
	TimeConstraintsList visibleConstraintsList;

	Constraint2ActivitiesGroupedForm();
	~Constraint2ActivitiesGroupedForm();

	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();

	bool filterOk(TimeConstraint* ctr);
};

#endif
