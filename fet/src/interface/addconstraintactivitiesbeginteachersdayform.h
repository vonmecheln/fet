/***************************************************************************
                          addconstraintactivitiesbeginteachersdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITIESBEGINTEACHERSDAYFORM_H
#define ADDCONSTRAINTACTIVITIESBEGINTEACHERSDAYFORM_H

#include "ui_addconstraintactivitiesbeginteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintActivitiesBeginTeachersDayForm : public QDialog, Ui::AddConstraintActivitiesBeginTeachersDayForm_template  {
	Q_OBJECT

public:
	AddConstraintActivitiesBeginTeachersDayForm(QWidget* parent);
	~AddConstraintActivitiesBeginTeachersDayForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox();
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

public slots:
	void addConstraint();
};

#endif
