/***************************************************************************
                          modifyconstraintmaxgapsbetweenactivitiesform.h  -  description
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

#ifndef MODIFYCONSTRAINTMAXGAPSBETWEENACTIVITIESFORM_H
#define MODIFYCONSTRAINTMAXGAPSBETWEENACTIVITIESFORM_H

#include "ui_modifyconstraintmaxgapsbetweenactivitiesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintMaxGapsBetweenActivitiesForm : public QDialog, Ui::ModifyConstraintMaxGapsBetweenActivitiesForm_template  {
	Q_OBJECT	

public:
	ModifyConstraintMaxGapsBetweenActivitiesForm(QWidget* parent, ConstraintMaxGapsBetweenActivities* ctr);
	~ModifyConstraintMaxGapsBetweenActivitiesForm();

	bool filterOk(Activity* ac);
	
public slots:
	void filterChanged();
	
	void addActivity();
	void addAllActivities();
	void removeActivity();

	void clear();

	void ok();
	void cancel();
	
private:
	ConstraintMaxGapsBetweenActivities* _ctr;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
