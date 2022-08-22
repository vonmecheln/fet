/***************************************************************************
                          addconstraintactivitybeginsteachersdayform.h  -  description
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

#ifndef ADDCONSTRAINTACTIVITYBEGINSTEACHERSDAYFORM_H
#define ADDCONSTRAINTACTIVITYBEGINSTEACHERSDAYFORM_H

#include "ui_addconstraintactivitybeginsteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivityBeginsTeachersDayForm : public QDialog, Ui::AddConstraintActivityBeginsTeachersDayForm_template  {
	Q_OBJECT
public:
	AddConstraintActivityBeginsTeachersDayForm(QWidget* parent);
	~AddConstraintActivityBeginsTeachersDayForm();

	bool filterOk(Activity* a);

public slots:
	void addCurrentConstraint();
	
	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
};

#endif
