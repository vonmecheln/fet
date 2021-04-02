/***************************************************************************
                          modifyconstraintteachermaxroomchangesperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERMAXROOMCHANGESPERWEEKFORM_H

#include "ui_modifyconstraintteachermaxroomchangesperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxRoomChangesPerWeekForm : public QDialog, Ui::ModifyConstraintTeacherMaxRoomChangesPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxRoomChangesPerWeek* _ctr;

	ModifyConstraintTeacherMaxRoomChangesPerWeekForm(QWidget* parent, ConstraintTeacherMaxRoomChangesPerWeek* ctr);
	~ModifyConstraintTeacherMaxRoomChangesPerWeekForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
