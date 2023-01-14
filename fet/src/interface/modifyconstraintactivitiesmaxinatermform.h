/***************************************************************************
                          modifyconstraintactivitiesmaxinatermform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTACTIVITIESMAXINATERMFORM_H
#define MODIFYCONSTRAINTACTIVITIESMAXINATERMFORM_H

#include "ui_modifyconstraintactivitiesmaxinatermform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesMaxInATermForm : public QDialog, Ui::ModifyConstraintActivitiesMaxInATermForm_template  {
	Q_OBJECT
	
public:
	ModifyConstraintActivitiesMaxInATermForm(QWidget* parent, ConstraintActivitiesMaxInATerm* ctr);
	~ModifyConstraintActivitiesMaxInATermForm();

	void updateActivitiesListWidget();
	
	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();
	
	void filterChanged();
	
	void clear();

	void ok();
	void cancel();

private:
	ConstraintActivitiesMaxInATerm* _ctr;

	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
