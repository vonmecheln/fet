/***************************************************************************
                          modifyconstraintstudentssetmaxroomchangesperdayinintervalform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXROOMCHANGESPERDAYININTERVALFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXROOMCHANGESPERDAYININTERVALFORM_H

#include "ui_modifyconstraintstudentssetmaxroomchangesperdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxRoomChangesPerDayInInterval* _ctr;

	ModifyConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm(QWidget* parent, ConstraintStudentsSetMaxRoomChangesPerDayInInterval* ctr);
	~ModifyConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
