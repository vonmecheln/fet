/***************************************************************************
                          modifyconstraintteachermaxroomchangesperdayinintervalform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERDAYININTERVALFORM_H
#define MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERDAYININTERVALFORM_H

#include "ui_modifyconstraintteachermaxroomchangesperdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm : public QDialog, Ui::ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxRoomChangesPerDayInInterval* _ctr;

	ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm(QWidget* parent, ConstraintTeacherMaxRoomChangesPerDayInInterval* ctr);
	~ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
