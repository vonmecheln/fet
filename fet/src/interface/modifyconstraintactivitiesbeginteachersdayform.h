/***************************************************************************
                          modifyconstraintactivitiesbeginteachersdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITIESBEGINTEACHERSDAYFORM_H
#define MODIFYCONSTRAINTACTIVITIESBEGINTEACHERSDAYFORM_H

#include "ui_modifyconstraintactivitiesbeginteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivitiesBeginTeachersDayForm : public QDialog, Ui::ModifyConstraintActivitiesBeginTeachersDayForm_template  {
	Q_OBJECT

public:
	ConstraintActivitiesBeginTeachersDay* _ctr;

	ModifyConstraintActivitiesBeginTeachersDayForm(QWidget* parent, ConstraintActivitiesBeginTeachersDay* ctr);
	~ModifyConstraintActivitiesBeginTeachersDayForm();

	void updateTeachersComboBox();
	void updateStudentsComboBox(QWidget* parent);
	void updateSubjectsComboBox();
	void updateActivityTagsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
