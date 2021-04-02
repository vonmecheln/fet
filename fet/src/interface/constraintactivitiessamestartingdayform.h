/***************************************************************************
                          constraintactivitiessamestartingdayform.h  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#ifndef CONSTRAINTACTIVITIESSAMESTARTINGDAYFORM_H
#define CONSTRAINTACTIVITIESSAMESTARTINGDAYFORM_H

#include "ui_constraintactivitiessamestartingdayform_template.h"
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
#include <q3listbox.h>

class ConstraintActivitiesSameStartingDayForm : public QDialog, Ui::ConstraintActivitiesSameStartingDayForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintActivitiesSameStartingDayForm();
	~ConstraintActivitiesSameStartingDayForm();

	void refreshConstraintsListBox();

	bool filterOk(TimeConstraint* ctr);
	
public slots:
	void constraintChanged(int index);
	void addConstraint();
	void removeConstraint();
	void modifyConstraint();
	
	void help();
};

#endif
