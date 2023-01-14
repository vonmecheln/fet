/***************************************************************************
                          modifyconstraintteacherafternoonsearlymaxbeginningsatsecondhourform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERAFTERNOONSEARLYMAXBEGINNINGSATSECONDHOURFORM_H
#define MODIFYCONSTRAINTTEACHERAFTERNOONSEARLYMAXBEGINNINGSATSECONDHOURFORM_H

#include "ui_modifyconstraintteacherafternoonsearlymaxbeginningsatsecondhourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm : public QDialog, Ui::ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm_template {
	Q_OBJECT
public:
	ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* _ctr;

	ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent, ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr);
	~ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm();

public slots:
	void ok();
	void cancel();
};

#endif
