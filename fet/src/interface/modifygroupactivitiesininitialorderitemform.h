/***************************************************************************
                          modifygroupactivitiesininitialorderitemform.h  -  description
                             -------------------
    begin                : 2014
    copyright            : (C) 2014 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYGROUPACTIVITIESININITIALORDERFORM_H
#define MODIFYGROUPACTIVITIESININITIALORDERFORM_H

#include "ui_modifygroupactivitiesininitialorderitemform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>

#include <QSet>
#include <QString>

class ModifyGroupActivitiesInInitialOrderItemForm : public QDialog, Ui::ModifyGroupActivitiesInInitialOrderItemForm_template  {
	Q_OBJECT

public:
	ModifyGroupActivitiesInInitialOrderItemForm(QWidget* parent, GroupActivitiesInInitialOrderItem* item);
	~ModifyGroupActivitiesInInitialOrderItemForm();

	bool filterOk(Activity* ac);
	
public Q_SLOTS:
	void addActivity();
	void addAllActivities();
	void removeActivity();

	void ok();
	void cancel();
	
	void clear();

	void filterChanged();
	
	void studentsFilterChanged();

private:
	QSet<QString> showedStudents;

	GroupActivitiesInInitialOrderItem* _item;
	//the id's of the activities listed in the activities list
	QList<int> activitiesList;
	//the id-s of the activities listed in the list of selected activities
	QList<int> selectedActivitiesList;
};

#endif
