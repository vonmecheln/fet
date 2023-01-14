/***************************************************************************
                          modifyconstraintteachermaxroomchangesperdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERDAYFORM_H
#define MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERDAYFORM_H

#include "ui_modifyconstraintteachermaxroomchangesperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxRoomChangesPerDayForm : public QDialog, Ui::ModifyConstraintTeacherMaxRoomChangesPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxRoomChangesPerDay* _ctr;

	ModifyConstraintTeacherMaxRoomChangesPerDayForm(QWidget* parent, ConstraintTeacherMaxRoomChangesPerDay* ctr);
	~ModifyConstraintTeacherMaxRoomChangesPerDayForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
