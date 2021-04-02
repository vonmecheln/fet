/***************************************************************************
                          modifyconstraintminimizebuildingchangesforstudentsform.h  -  description
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

#ifndef MODIFYCONSTRAINTMINIMIZEBUILDINGCHANGESFORSTUDENTSFORM_H
#define MODIFYCONSTRAINTMINIMIZEBUILDINGCHANGESFORSTUDENTSFORM_H

#include "modifyconstraintminimizebuildingchangesforstudentsform_template.h"
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

class ModifyConstraintMinimizeBuildingChangesForStudentsForm : public ModifyConstraintMinimizeBuildingChangesForStudentsForm_template  {
public:
	ConstraintMinimizeBuildingChangesForStudents* _ctr;

	ModifyConstraintMinimizeBuildingChangesForStudentsForm(ConstraintMinimizeBuildingChangesForStudents* ctr);
	~ModifyConstraintMinimizeBuildingChangesForStudentsForm();

	void constraintChanged();
	void ok();
	void cancel();
};

#endif
