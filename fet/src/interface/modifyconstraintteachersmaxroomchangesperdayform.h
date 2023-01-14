/***************************************************************************
                          modifyconstraintteachersmaxroomchangesperdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXROOMCHANGESPERDAYFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXROOMCHANGESPERDAYFORM_H

#include "ui_modifyconstraintteachersmaxroomchangesperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxRoomChangesPerDayForm : public QDialog, Ui::ModifyConstraintTeachersMaxRoomChangesPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMaxRoomChangesPerDay* _ctr;

	ModifyConstraintTeachersMaxRoomChangesPerDayForm(QWidget* parent, ConstraintTeachersMaxRoomChangesPerDay* ctr);
	~ModifyConstraintTeachersMaxRoomChangesPerDayForm();

public slots:
	void ok();
	void cancel();
};

#endif
