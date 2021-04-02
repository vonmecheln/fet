/***************************************************************************
                          modifyconstraintstudentsmaxroomchangesperdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXROOMCHANGESPERDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXROOMCHANGESPERDAYFORM_H

#include "ui_modifyconstraintstudentsmaxroomchangesperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxRoomChangesPerDayForm : public QDialog, Ui::ModifyConstraintStudentsMaxRoomChangesPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxRoomChangesPerDay* _ctr;

	ModifyConstraintStudentsMaxRoomChangesPerDayForm(QWidget* parent, ConstraintStudentsMaxRoomChangesPerDay* ctr);
	~ModifyConstraintStudentsMaxRoomChangesPerDayForm();

public slots:
	void ok();
	void cancel();
};

#endif
