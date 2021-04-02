/***************************************************************************
                          modifyconstraintactivitypreferredstartingtimeform.h  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMEFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMEFORM_H

#include "modifyconstraintactivitypreferredstartingtimeform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class ModifyConstraintActivityPreferredStartingTimeForm : public ModifyConstraintActivityPreferredStartingTimeForm_template  {
public:
	ConstraintActivityPreferredStartingTime* _ctr;

	ModifyConstraintActivityPreferredStartingTimeForm(ConstraintActivityPreferredStartingTime* ctr);
	~ModifyConstraintActivityPreferredStartingTimeForm();

	void updatePeriodGroupBox();
	void updateActivitiesComboBox();

	void constraintChanged();
	void ok();
	void cancel();

	bool filterOk(Activity* a);
	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;

};

#endif
