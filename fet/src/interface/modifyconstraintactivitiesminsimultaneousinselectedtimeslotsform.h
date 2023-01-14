/***************************************************************************
                          modifyconstraintactivitiesminsimultaneousinselectedtimeslotsform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTACTIVITIESMINSIMULTANEOUSINSELECTEDTIMESLOTSFORM_H
#define MODIFYCONSTRAINTACTIVITIESMINSIMULTANEOUSINSELECTEDTIMESLOTSFORM_H

#include "ui_modifyconstraintactivitiesminsimultaneousinselectedtimeslotsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm : public QDialog, Ui::ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm_template  {
	Q_OBJECT
	
public:
	ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm(QWidget* parent, ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* ctr);
	~ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm();

	void colorItem(QTableWidgetItem* item);
	
	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();
	
	void filterChanged();
	
	void clear();

	void ok();
	void cancel();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void setAllUnselected();
	void setAllSelected();
	
private:
	ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* _ctr;

	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
