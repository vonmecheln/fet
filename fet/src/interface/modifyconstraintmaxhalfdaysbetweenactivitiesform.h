/***************************************************************************
                          modifyconstraintmahalfdaysbetweenactivitiesform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTMAXHALFDAYSBETWEENACTIVITIESFORM_H
#define MODIFYCONSTRAINTMAXHALFDAYSBETWEENACTIVITIESFORM_H

#include "ui_modifyconstraintmaxhalfdaysbetweenactivitiesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintMaxHalfDaysBetweenActivitiesForm : public QDialog, Ui::ModifyConstraintMaxHalfDaysBetweenActivitiesForm_template  {
	Q_OBJECT

public:
	ModifyConstraintMaxHalfDaysBetweenActivitiesForm(QWidget* parent, ConstraintMaxHalfDaysBetweenActivities* ctr);
	~ModifyConstraintMaxHalfDaysBetweenActivitiesForm();

	bool filterOk(Activity* ac);
	
public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();

	void clear();

	void ok();
	void cancel();
	
	void filterChanged();

private:
	ConstraintMaxHalfDaysBetweenActivities* _ctr;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
