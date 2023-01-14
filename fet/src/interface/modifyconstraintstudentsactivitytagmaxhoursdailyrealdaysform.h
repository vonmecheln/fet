/***************************************************************************
                          modifyconstraintstudentsactivitytagmaxhoursdailyrealdaysform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYCONSTRAINTSTUDENTSACTIVITYTAGMAXHOURSDAILYREALDAYSFORM_H
#define MODIFYCONSTRAINTSTUDENTSACTIVITYTAGMAXHOURSDAILYREALDAYSFORM_H

#include "ui_modifyconstraintstudentsactivitytagmaxhoursdailyrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsActivityTagMaxHoursDailyRealDaysForm : public QDialog, Ui::ModifyConstraintStudentsActivityTagMaxHoursDailyRealDaysForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsActivityTagMaxHoursDailyRealDays* _ctr;

	ModifyConstraintStudentsActivityTagMaxHoursDailyRealDaysForm(QWidget* parent, ConstraintStudentsActivityTagMaxHoursDailyRealDays* ctr);
	~ModifyConstraintStudentsActivityTagMaxHoursDailyRealDaysForm();

	void updateActivityTagsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
