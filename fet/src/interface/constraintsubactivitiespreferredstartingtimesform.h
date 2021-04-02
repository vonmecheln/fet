/***************************************************************************
                          constraintsubactivitiespreferredstartingtimes.h  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#ifndef CONSTRAINTSUBACTIVITIESPREFERREDSTARTINGTIMESFORM_H
#define CONSTRAINTSUBACTIVITIESPREFERREDSTARTINGTIMESFORM_H

#include "ui_constraintsubactivitiespreferredstartingtimesform_template.h"
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

class ConstraintSubactivitiesPreferredStartingTimesForm : public QDialog, Ui::ConstraintSubactivitiesPreferredStartingTimesForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintSubactivitiesPreferredStartingTimesForm();
	~ConstraintSubactivitiesPreferredStartingTimesForm();

	void refreshConstraintsListBox();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void help();
};

#endif
