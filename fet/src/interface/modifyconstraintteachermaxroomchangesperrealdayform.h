/***************************************************************************
                          modifyconstraintteachermaxroomchangesperrealdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERREALDAYFORM_H
#define MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERREALDAYFORM_H

#include "ui_modifyconstraintteachermaxroomchangesperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxRoomChangesPerRealDayForm : public QDialog, Ui::ModifyConstraintTeacherMaxRoomChangesPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxRoomChangesPerRealDay* _ctr;

	ModifyConstraintTeacherMaxRoomChangesPerRealDayForm(QWidget* parent, ConstraintTeacherMaxRoomChangesPerRealDay* ctr);
	~ModifyConstraintTeacherMaxRoomChangesPerRealDayForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
