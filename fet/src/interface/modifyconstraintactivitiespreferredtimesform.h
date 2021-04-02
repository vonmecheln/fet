/***************************************************************************
                          modifyconstraintactivitiespreferredtimesform.h  -  description
                             -------------------
    begin                : 11 Feb 2005
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

#ifndef MODIFYCONSTRAINTACTIVITIESPREFERREDTIMESFORM_H
#define MODIFYCONSTRAINTACTIVITIESPREFERREDTIMESFORM_H

#include "modifyconstraintactivitiespreferredtimesform_template.h"
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
#include <qpoint.h>

class ModifyConstraintActivitiesPreferredTimesForm : public ModifyConstraintActivitiesPreferredTimesForm_template  {
public:
	ConstraintActivitiesPreferredTimes* _ctr;

	ModifyConstraintActivitiesPreferredTimesForm(ConstraintActivitiesPreferredTimes* ctr);
	~ModifyConstraintActivitiesPreferredTimesForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox();
	void updateSubjectsComboBox();
	void updateSubjectTagsComboBox();

	void ok();
	void cancel();
	
	void tableClicked(int, int, int, const QPoint&);
};

#endif
