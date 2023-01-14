/***************************************************************************
                          modifyconstraintstudentsmingapsbetweenroomchangesform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENROOMCHANGESFORM_H
#define MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENROOMCHANGESFORM_H

#include "ui_modifyconstraintstudentsmingapsbetweenroomchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMinGapsBetweenRoomChangesForm : public QDialog, Ui::ModifyConstraintStudentsMinGapsBetweenRoomChangesForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMinGapsBetweenRoomChanges* _ctr;

	ModifyConstraintStudentsMinGapsBetweenRoomChangesForm(QWidget* parent, ConstraintStudentsMinGapsBetweenRoomChanges* ctr);
	~ModifyConstraintStudentsMinGapsBetweenRoomChangesForm();

public slots:
	void ok();
	void cancel();
};

#endif
