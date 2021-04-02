/***************************************************************************
                          constraintminimizebuildingchangesforteachersform.h  -  description
                             -------------------
    begin                : July 9, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef CONSTRAINTMINIMIZEBUILDINGCHANGESFORTEACHERSFORM_H
#define CONSTRAINTMINIMIZEBUILDINGCHANGESFORTEACHERSFORM_H

#include "constraintminimizebuildingchangesforteachersform_template.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"
#include "fetmainform.h"

#include <qcombobox.h>
#include <qmessagebox.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlistbox.h>

class ConstraintMinimizeBuildingChangesForTeachersForm : public ConstraintMinimizeBuildingChangesForTeachersForm_template  {
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintMinimizeBuildingChangesForTeachersForm();
	~ConstraintMinimizeBuildingChangesForTeachersForm();

	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();

	bool filterOk(SpaceConstraint* ctr);
};

#endif
