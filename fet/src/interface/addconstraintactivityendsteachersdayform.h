/***************************************************************************
                          addconstraintactivityendsteachersdayform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTACTIVITYENDSTEACHERSDAYFORM_H
#define ADDCONSTRAINTACTIVITYENDSTEACHERSDAYFORM_H

#include "ui_addconstraintactivityendsteachersdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintActivityEndsTeachersDayForm : public QDialog, Ui::AddConstraintActivityEndsTeachersDayForm_template  {
	Q_OBJECT
public:
	AddConstraintActivityEndsTeachersDayForm(QWidget* parent);
	~AddConstraintActivityEndsTeachersDayForm();

	bool filterOk(Activity* a);

public slots:
	void addCurrentConstraint();
	
	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
};

#endif
