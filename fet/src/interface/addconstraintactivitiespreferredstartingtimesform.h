/***************************************************************************
                          addconstraintactivitiespreferredstartingtimesform.h  -  description
                             -------------------
    begin                : 15 May 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITIESPREFERREDSTARTINGTIMESFORM_H
#define ADDCONSTRAINTACTIVITIESPREFERREDSTARTINGTIMESFORM_H

#include "ui_addconstraintactivitiespreferredstartingtimesform_template.h"
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
#include <qpoint.h>

class AddConstraintActivitiesPreferredStartingTimesForm : public QDialog, Ui::AddConstraintActivitiesPreferredStartingTimesForm_template  {
	Q_OBJECT

public:
	AddConstraintActivitiesPreferredStartingTimesForm();
	~AddConstraintActivitiesPreferredStartingTimesForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox();
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();
	
	void colorItem(QTableWidgetItem* item);

public slots:
	void addConstraint();
	
	void setAllSlotsAllowed();
	void setAllSlotsNotAllowed();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
};

#endif
