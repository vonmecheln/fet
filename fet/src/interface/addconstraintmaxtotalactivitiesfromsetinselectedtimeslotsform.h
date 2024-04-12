/***************************************************************************
                          addconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform.h  -  description
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

#ifndef ADDCONSTRAINTMAXTOTALACTIVITIESFROMSETINSELECTEDTIMESLOTSFORM_H
#define ADDCONSTRAINTMAXTOTALACTIVITIESFROMSETINSELECTEDTIMESLOTSFORM_H

#include "ui_addconstraintmaxtotalactivitiesfromsetinselectedtimeslotsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class AddConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm : public QDialog, Ui::AddConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm_template  {
	Q_OBJECT
	
public:
	AddConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm(QWidget* parent);
	~AddConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm();

	void colorItem(QTableWidgetItem* item);
	
	bool filterOk(Activity* act);

public slots:
	void addActivity();
	void addAllActivities();
	void removeActivity();
	
	void filterChanged();
	
	void clear();

	void addCurrentConstraint();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void cellEntered(int row, int col);
	
	void setAllUnselected();
	void setAllSelected();
	
private:
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
