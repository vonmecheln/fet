/***************************************************************************
                          subactivitiesform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBACTIVITIESFORM_H
#define SUBACTIVITIESFORM_H

#include "ui_subactivitiesform_template.h"

#include "activity.h"

#include "advancedfilterform.h"

#include <QSet>
#include <QString>

class SubactivitiesForm : public QDialog, Ui::SubactivitiesForm_template  {
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
	ActivitiesList visibleSubactivitiesList;

	SubactivitiesForm(QWidget* parent, const QString& teacherName, const QString& studentsSetName, const QString& subjectName, const QString& activityTagName);
	~SubactivitiesForm();

	bool filterOk(Activity* act);

public Q_SLOTS:
	void modifySubactivity();
	void subactivityChanged();
	void filterChanged();

	void filter(bool active);
	
	void studentsFilterChanged();
	
	void help();

	void activateSubactivity();
	void deactivateSubactivity();
	void activateAllSubactivities();
	void deactivateAllSubactivities();
	
	void subactivityComments();
};

#endif
