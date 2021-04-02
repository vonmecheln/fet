/***************************************************************************
                          modifyconstraintstudentssetactivitytagminhoursdailyform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETACTIVITYTAGMINHOURSDAILYFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETACTIVITYTAGMINHOURSDAILYFORM_H

#include "ui_modifyconstraintstudentssetactivitytagminhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetActivityTagMinHoursDailyForm : public QDialog, Ui::ModifyConstraintStudentsSetActivityTagMinHoursDailyForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetActivityTagMinHoursDaily* _ctr;

	ModifyConstraintStudentsSetActivityTagMinHoursDailyForm(QWidget* parent, ConstraintStudentsSetActivityTagMinHoursDaily* ctr);
	~ModifyConstraintStudentsSetActivityTagMinHoursDailyForm();

	void updateStudentsComboBox(QWidget* parent);
	void updateActivityTagsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
