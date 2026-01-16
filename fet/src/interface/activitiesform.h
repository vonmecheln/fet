/***************************************************************************
                          activitiesform.h  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITIESFORM_H
#define ACTIVITIESFORM_H

#include "ui_activitiesform_template.h"

#include "activity.h"

#include "advancedfilterform.h"

#include <QSet>
#include <QString>

class ActivitiesForm : public QDialog, Ui::ActivitiesForm_template{
	Q_OBJECT
	
private:
	QSet<QString> showedStudents;

	AdvancedFilterForm* filterForm;

	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescrDetDescrWithConstraints;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;
	
	int NA;
	int NT;
	int DA;
	int DT;
	
public:
	ActivitiesList visibleActivitiesList;

	ActivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName);
	~ActivitiesForm();

	bool filterOk(Activity* act);

public Q_SLOTS:
	void addActivity();
	void removeActivities();
	void modifyActivity();
	void activityChanged();
	void filterChanged();
	
	void filter(bool active);
	
	void studentsFilterChanged();
	
	void help();
	
	void activateActivities();
	void deactivateActivities();
	//void activateAllActivities();
	//void deactivateAllActivities();

	void activityComments();
	
	void selectionChanged();
	
	void changeActivityTags();
};

#endif
