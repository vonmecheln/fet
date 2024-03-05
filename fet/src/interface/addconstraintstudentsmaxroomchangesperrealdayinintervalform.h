/***************************************************************************
                          addconstraintstudentsmaxroomchangesperrealdayinintervalform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMAXROOMCHANGESPERREALDAYININTERVALFORM_H
#define ADDCONSTRAINTSTUDENTSMAXROOMCHANGESPERREALDAYININTERVALFORM_H

#include "ui_addconstraintstudentsmaxroomchangesperrealdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm : public QDialog, Ui::AddConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm(QWidget* parent);
	~AddConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm();

public slots:
	void addCurrentConstraint();
};

#endif
