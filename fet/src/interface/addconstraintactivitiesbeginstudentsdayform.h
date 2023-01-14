/***************************************************************************
                          addconstraintactivitiesbeginstudentsdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTACTIVITIESBEGINSTUDENTSDAYFORM_H
#define ADDCONSTRAINTACTIVITIESBEGINSTUDENTSDAYFORM_H

#include "ui_addconstraintactivitiesbeginstudentsdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintActivitiesBeginStudentsDayForm : public QDialog, Ui::AddConstraintActivitiesBeginStudentsDayForm_template  {
	Q_OBJECT

public:
	AddConstraintActivitiesBeginStudentsDayForm(QWidget* parent);
	~AddConstraintActivitiesBeginStudentsDayForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox();
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

public slots:
	void addConstraint();
};

#endif
