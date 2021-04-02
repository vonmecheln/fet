/***************************************************************************
                          addconstraintactivitiessamestartinghourform.h  -  description
                             -------------------
    begin                : Feb 15, 2005
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

#ifndef ADDCONSTRAINTACTIVITIESSAMESTARTINGHOURFORM_H
#define ADDCONSTRAINTACTIVITIESSAMESTARTINGHOURFORM_H

#include "addconstraintactivitiessamestartinghourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"

#include <q3combobox.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

#include <QList>

class AddConstraintActivitiesSameStartingHourForm : public AddConstraintActivitiesSameStartingHourForm_template  {
public:
	AddConstraintActivitiesSameStartingHourForm();
	~AddConstraintActivitiesSameStartingHourForm();

	void updateActivitiesListBox();
	
	void addActivity();
	void removeActivity();

	void addConstraint();
	
private:
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of same starting hour activities
	QList<int> selectedActivitiesList;
};

#endif
