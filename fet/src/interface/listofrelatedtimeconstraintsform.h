/***************************************************************************
                          listofrelatedtimeconstraintsform.h  -  description
                             -------------------
    begin                : 2025
    copyright            : (C) 2025 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef LISTOFRELATEDTIMECONSTRAINTSFORM_H
#define LISTOFRELATEDTIMECONSTRAINTSFORM_H

#include "ui_listofrelatedtimeconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

extern const int FILTER_IS_UNSPECIFIED;
extern const int FILTER_IS_TEACHER;
extern const int FILTER_IS_STUDENTS_SET;
extern const int FILTER_IS_ROOM;
extern const int FILTER_IS_ACTIVITY;

class ListOfRelatedTimeConstraintsForm : public QDialog, Ui::ListOfRelatedTimeConstraintsForm_template  {
	Q_OBJECT
	
private:
	int filterBy;
	QList<int> filterId;
	QString filterName;
	QList<TimeConstraint*> allRelatedTimeConstraintsList;

	AdvancedFilterForm* filterForm;

	QList<TimeConstraint*> visibleTimeConstraintsList;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;
	
public:
	ListOfRelatedTimeConstraintsForm(QWidget* parent, int _filterBy, const QList<int>& _filterId, const QString& _filterName, const QList<TimeConstraint*>& _allRelatedTimeConstraintsList);
	~ListOfRelatedTimeConstraintsForm();
	
	bool filterOk(TimeConstraint* ctr);
	
	void filterChanged();
	
public Q_SLOTS:
	void constraintChanged();
	void modifyConstraint();
	void removeConstraints();
	void filter(bool active);

	void moveTimeConstraintUp();
	void moveTimeConstraintDown();

	void sortedChanged(bool checked);
	
	void activateConstraints();
	void deactivateConstraints();
	void constraintComments();
	
	void changeWeights();
	
	void selectionChanged();
	
	void addConstraint();

	void useColorsChanged();
};

#endif
