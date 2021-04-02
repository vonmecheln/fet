/***************************************************************************
                          allspaceconstraintsform.h  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#ifndef ALLSPACECONSTRAINTSFORM_H
#define ALLSPACECONSTRAINTSFORM_H

#include "allspaceconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3listbox.h>

class AllSpaceConstraintsForm : public AllSpaceConstraintsForm_template  {
public:
	SpaceConstraintsList visibleConstraintsList;

	AllSpaceConstraintsForm();
	~AllSpaceConstraintsForm();

	void constraintChanged(int index);
	void modifyConstraint();
	void removeConstraint();
	
	void filterChanged();

	bool filterOk(SpaceConstraint* ctr);
};

#endif
