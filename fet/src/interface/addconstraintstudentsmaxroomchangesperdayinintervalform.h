/***************************************************************************
                          addconstraintstudentsmaxroomchangesperdayinintervalform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMAXROOMCHANGESPERDAYININTERVALFORM_H
#define ADDCONSTRAINTSTUDENTSMAXROOMCHANGESPERDAYININTERVALFORM_H

#include "ui_addconstraintstudentsmaxroomchangesperdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMaxRoomChangesPerDayInIntervalForm : public QDialog, Ui::AddConstraintStudentsMaxRoomChangesPerDayInIntervalForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMaxRoomChangesPerDayInIntervalForm(QWidget* parent);
	~AddConstraintStudentsMaxRoomChangesPerDayInIntervalForm();

public slots:
	void addCurrentConstraint();
};

#endif
