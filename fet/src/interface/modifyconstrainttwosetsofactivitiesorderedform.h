/***************************************************************************
                          modifyconstrainttwosetsofactivitiesorderedform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTWOSETSOFACTIVITIESORDEREDFORM_H
#define MODIFYCONSTRAINTTWOSETSOFACTIVITIESORDEREDFORM_H

#include "ui_modifyconstrainttwosetsofactivitiesorderedform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

class ModifyConstraintTwoSetsOfActivitiesOrderedForm : public QDialog, Ui::ModifyConstraintTwoSetsOfActivitiesOrderedForm_template  {
	Q_OBJECT
public:
	ModifyConstraintTwoSetsOfActivitiesOrderedForm(QWidget* parent, ConstraintTwoSetsOfActivitiesOrdered* ctr);
	~ModifyConstraintTwoSetsOfActivitiesOrderedForm();

	bool filterOk(Activity* a);

public slots:
	void filterChanged();

	void firstAddActivity();
	void firstAddAllActivities();
	void firstRemoveActivity();

	void firstClear();

	void secondAddActivity();
	void secondAddAllActivities();
	void secondRemoveActivity();

	void secondClear();

	void ok();
	void cancel();
	
	void swap();
	
private:
	ConstraintTwoSetsOfActivitiesOrdered* _ctr;

	//the id's of the activities listed in the first activities list
	QList<int> firstActivitiesList;
	//the id-s of the activities listed in the list of first selected activities
	QList<int> firstSelectedActivitiesList;

	//the id's of the activities listed in the second activities list
	QList<int> secondActivitiesList;
	//the id-s of the activities listed in the list of second selected activities
	QList<int> secondSelectedActivitiesList;
};

#endif
