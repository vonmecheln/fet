/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhoursdailyform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSDAILYFORM_H
#define ADDCONSTRAINTSTUDENTSSETACTIVITYTAGMAXHOURSDAILYFORM_H

#include "ui_addconstraintstudentssetactivitytagmaxhoursdailyform_template.h"
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

class AddConstraintStudentsSetActivityTagMaxHoursDailyForm : public QDialog, Ui::AddConstraintStudentsSetActivityTagMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetActivityTagMaxHoursDailyForm();
	~AddConstraintStudentsSetActivityTagMaxHoursDailyForm();
	
	void updateStudentsSetComboBox();
	void updateActivityTagsComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
