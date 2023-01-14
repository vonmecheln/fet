/***************************************************************************
                          modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSMORNINGSEARLYMAXBEGINNINGSATSECONDHOURFORM_H
#define MODIFYCONSTRAINTSTUDENTSMORNINGSEARLYMAXBEGINNINGSATSECONDHOURFORM_H

#include "ui_modifyconstraintstudentsmorningsearlymaxbeginningsatsecondhourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm : public QDialog, Ui::ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour* _ctr;

	ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent, ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour* ctr);
	~ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm();

public slots:
	void ok();
	void cancel();
};

#endif
