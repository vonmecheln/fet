/***************************************************************************
                          constraintteachersmingapsbetweenorderedpairofactivitytagsform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#ifndef CONSTRAINTTEACHERSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H
#define CONSTRAINTTEACHERSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H

#include "ui_constraintteachersmingapsbetweenorderedpairofactivitytagsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm : public QDialog, Ui::ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm_template  {
	Q_OBJECT
public:
	TimeConstraintsList visibleConstraintsList;

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm(QWidget* parent);
	~ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm();

	bool filterOk(TimeConstraint* ctr);

public slots:
	void constraintChanged(int index);
	void addConstraint();
	void modifyConstraint();
	void removeConstraint();

	void filterChanged();
};

#endif
