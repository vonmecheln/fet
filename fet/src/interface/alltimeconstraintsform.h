/***************************************************************************
                          alltimeconstraintsform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ALLTIMECONSTRAINTSFORM_H
#define ALLTIMECONSTRAINTSFORM_H

#include "ui_alltimeconstraintsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "advancedfilterform.h"

class AllTimeConstraintsForm : public QDialog, Ui::AllTimeConstraintsForm_template  {
	Q_OBJECT
	
private:
	AdvancedFilterForm* filterForm;

	QList<TimeConstraint*> visibleTimeConstraintsList;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool useFilter;
	
public:
	AllTimeConstraintsForm(QWidget* parent);
	~AllTimeConstraintsForm();
	
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
	//void activateAllConstraints();
	//void deactivateAllConstraints();
	void constraintComments();
	
	void changeWeights();
	
	void selectionChanged();
};

#endif
