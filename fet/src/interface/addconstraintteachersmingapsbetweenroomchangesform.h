/***************************************************************************
                          addconstraintteachersmingapsbetweenroomchangesform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERSMINGAPSBETWEENROOMCHANGESFORM_H
#define ADDCONSTRAINTTEACHERSMINGAPSBETWEENROOMCHANGESFORM_H

#include "ui_addconstraintteachersmingapsbetweenroomchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMinGapsBetweenRoomChangesForm : public QDialog, Ui::AddConstraintTeachersMinGapsBetweenRoomChangesForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMinGapsBetweenRoomChangesForm(QWidget* parent);
	~AddConstraintTeachersMinGapsBetweenRoomChangesForm();

public slots:
	void addCurrentConstraint();
};

#endif
