/***************************************************************************
                          modifyconstraintstudentssetmaxroomchangesperrealdayinintervalform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXROOMCHANGESPERREALDAYININTERVALFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXROOMCHANGESPERREALDAYININTERVALFORM_H

#include "ui_modifyconstraintstudentssetmaxroomchangesperrealdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval* _ctr;

	ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm(QWidget* parent, ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval* ctr);
	~ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
