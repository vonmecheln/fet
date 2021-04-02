/***************************************************************************
                          addconstraint2activitiesconsecutiveform.h  -  description
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

#ifndef ADDCONSTRAINT2ACTIVITIESCONSECUTIVEFORM_H
#define ADDCONSTRAINT2ACTIVITIESCONSECUTIVEFORM_H

#include "addconstraint2activitiesconsecutiveform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

#include <QList>

class AddConstraint2ActivitiesConsecutiveForm : public AddConstraint2ActivitiesConsecutiveForm_template  {
public:
	AddConstraint2ActivitiesConsecutiveForm();
	~AddConstraint2ActivitiesConsecutiveForm();

	void updateActivitiesComboBox();

	void constraintChanged();
	void addCurrentConstraint();
	
	bool filterOk(Activity* a);
	void filterChanged();
	
private:
	//the id's of the activities listed in the first and second activities combo
	QList<int> firstActivitiesList;
	QList<int> secondActivitiesList;
};

#endif
