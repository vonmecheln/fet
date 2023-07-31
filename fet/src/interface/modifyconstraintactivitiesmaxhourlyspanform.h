/***************************************************************************
                          modifyconstraintactivitiesmaxhourlyspanform.h  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTACTIVITIESMAXHOURLYSPANFORM_H
#define MODIFYCONSTRAINTACTIVITIESMAXHOURLYSPANFORM_H

#include "ui_modifyconstraintactivitiesmaxhourlyspanform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesMaxHourlySpanForm : public QDialog, Ui::ModifyConstraintActivitiesMaxHourlySpanForm_template  {
	Q_OBJECT

public:
	ModifyConstraintActivitiesMaxHourlySpanForm(QWidget* parent, ConstraintActivitiesMaxHourlySpan* ctr);
	~ModifyConstraintActivitiesMaxHourlySpanForm();

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
	ConstraintActivitiesMaxHourlySpan* _ctr;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
