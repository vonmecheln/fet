/***************************************************************************
                          constraintthreeactivitiesgroupedform.h  -  description
                             -------------------
    begin                : Aug 14, 2009
    copyright            : (C) 2009 by Liviu Lalescu
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

#ifndef CONSTRAINTTHREEACTIVITIESGROUPEDFORM_H
#define CONSTRAINTTHREEACTIVITIESGROUPEDFORM_H

#include "ui_constraintthreeactivitiesgroupedform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintThreeActivitiesGroupedForm : public QDialog, Ui::ConstraintThreeActivitiesGroupedForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintThreeActivitiesGroupedForm(QWidget* parent);
	~ConstraintThreeActivitiesGroupedForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
