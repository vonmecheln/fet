/***************************************************************************
                          constraintactivitiessameroomform.h  -  description
                             -------------------
    begin                : 28 March 2005
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

#ifndef CONSTRAINTACTIVITIESSAMEROOMFORM_H
#define CONSTRAINTACTIVITIESSAMEROOMFORM_H

#include "constraintactivitiessameroomform_template.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"
#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3listbox.h>

class ConstraintActivitiesSameRoomForm : public ConstraintActivitiesSameRoomForm_template  {
public:
	SpaceConstraintsList visibleConstraintsList;

	ConstraintActivitiesSameRoomForm();
	~ConstraintActivitiesSameRoomForm();

	void constraintChanged(int index);
	void addConstraint();
	void removeConstraint();
	void modifyConstraint();
	
	void refreshConstraintsListBox();

	bool filterOk(SpaceConstraint* ctr);
};

#endif