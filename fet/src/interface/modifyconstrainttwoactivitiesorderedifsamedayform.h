/***************************************************************************
                          modifyconstrainttwoactivitiesorderedifsamedayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTWOACTIVITIESORDEREDIFSAMEDAYFORM_H
#define MODIFYCONSTRAINTTWOACTIVITIESORDEREDIFSAMEDAYFORM_H

#include "ui_modifyconstrainttwoactivitiesorderedifsamedayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTwoActivitiesOrderedIfSameDayForm : public QDialog, Ui::ModifyConstraintTwoActivitiesOrderedIfSameDayForm_template  {
	Q_OBJECT
public:
	ConstraintTwoActivitiesOrderedIfSameDay* _ctr;

	ModifyConstraintTwoActivitiesOrderedIfSameDayForm(QWidget* parent, ConstraintTwoActivitiesOrderedIfSameDay* ctr);
	~ModifyConstraintTwoActivitiesOrderedIfSameDayForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

public slots:
	void ok();
	void cancel();

	void swap();
	
	void filterChanged();
	
private:
	//the id's of the activities listed in the activities combo
	QList<int> firstActivitiesList;
	QList<int> secondActivitiesList;
};

#endif
