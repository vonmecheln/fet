/***************************************************************************
                          groupactivitiesininitialorderitemsform.h  -  description
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

#ifndef GROUPACTIVITIESININITIALORDERITEMFORM_H
#define GROUPACTIVITIESININITIALORDERITEMFORM_H

#include "ui_groupactivitiesininitialorderitemsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

#include <QSet>
#include <QString>

class GroupActivitiesInInitialOrderItemsForm : public QDialog, Ui::GroupActivitiesInInitialOrderItemsForm_template  {
	Q_OBJECT

private:
	QSet<QString> showedStudents;

	AdvancedFilterForm* filterForm;

	QList<GroupActivitiesInInitialOrderItem*> visibleItemsList;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;

public:
	GroupActivitiesInInitialOrderItemsForm(QWidget* parent);
	~GroupActivitiesInInitialOrderItemsForm();

	bool filterOk(GroupActivitiesInInitialOrderItem* item);
	
public Q_SLOTS:
	void itemChanged(int index);
	void addItem();
	void modifyItem();
	void removeItem();
	void filterChanged();
	
	void studentsFilterChanged();
	
	void filter(bool active);
	
	void moveItemUp();
	void moveItemDown();
	
	void help();

	void activateItem();
	void deactivateItem();
	void activateAllItems();
	void deactivateAllItems();
	void itemComments();
};

#endif
