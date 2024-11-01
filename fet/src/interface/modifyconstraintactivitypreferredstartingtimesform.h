/***************************************************************************
                          modifyconstraintactivitypreferredstartingtimesform.h  -  description
                             -------------------
    begin                : Feb 14, 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMESFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDSTARTINGTIMESFORM_H

#include "ui_modifyconstraintactivitypreferredstartingtimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintActivityPreferredStartingTimesForm : public QDialog, Ui::ModifyConstraintActivityPreferredStartingTimesForm_template  {
	Q_OBJECT

	ConstraintActivityPreferredStartingTimes* _ctr;
public:
	ModifyConstraintActivityPreferredStartingTimesForm(QWidget* parent, ConstraintActivityPreferredStartingTimes* ctr);
	~ModifyConstraintActivityPreferredStartingTimesForm();

	void updateActivitiesComboBox();

	bool filterOk(Activity* a);

	void colorItem(QTableWidgetItem* item);

private:
	//the id's of the activities listed in the activities combo
	QList<int> activitiesList;
	
public slots:
	void filterChanged();
	
	void ok();
	void cancel();
	
	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	
	void cellEntered(int row, int col);
	
	void setAllSlotsAllowed();
	void setAllSlotsNotAllowed();
};

#endif
