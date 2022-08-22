/***************************************************************************
                          modifyconstraintactivitybeginsteachersdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITYBEGINSTEACHERSDAYFORM_H
#define MODIFYCONSTRAINTACTIVITYBEGINSTEACHERSDAYFORM_H

#include "ui_modifyconstraintactivitybeginsteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityBeginsTeachersDayForm : public QDialog, Ui::ModifyConstraintActivityBeginsTeachersDayForm_template  {
	Q_OBJECT
public:
	ConstraintActivityBeginsTeachersDay* _ctr;

	ModifyConstraintActivityBeginsTeachersDayForm(QWidget* parent, ConstraintActivityBeginsTeachersDay* ctr);
	~ModifyConstraintActivityBeginsTeachersDayForm();

	bool filterOk(Activity* a);

public slots:
	void ok();
	void cancel();

	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
};

#endif
