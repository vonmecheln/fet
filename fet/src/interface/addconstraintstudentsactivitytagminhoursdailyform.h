/***************************************************************************
                          addconstraintstudentsactivitytagminhoursdailyform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTSTUDENTSACTIVITYTAGMINHOURSDAILYFORM_H
#define ADDCONSTRAINTSTUDENTSACTIVITYTAGMINHOURSDAILYFORM_H

#include "ui_addconstraintstudentsactivitytagminhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsActivityTagMinHoursDailyForm : public QDialog, Ui::AddConstraintStudentsActivityTagMinHoursDailyForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsActivityTagMinHoursDailyForm(QWidget* parent);
	~AddConstraintStudentsActivityTagMinHoursDailyForm();
	
	void updateActivityTagsComboBox();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
