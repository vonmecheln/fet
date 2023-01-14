/***************************************************************************
                          modifyconstraintstudentssetmingapsbetweenroomchangesform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMINGAPSBETWEENROOMCHANGESFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMINGAPSBETWEENROOMCHANGESFORM_H

#include "ui_modifyconstraintstudentssetmingapsbetweenroomchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMinGapsBetweenRoomChangesForm : public QDialog, Ui::ModifyConstraintStudentsSetMinGapsBetweenRoomChangesForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMinGapsBetweenRoomChanges* _ctr;

	ModifyConstraintStudentsSetMinGapsBetweenRoomChangesForm(QWidget* parent, ConstraintStudentsSetMinGapsBetweenRoomChanges* ctr);
	~ModifyConstraintStudentsSetMinGapsBetweenRoomChangesForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
