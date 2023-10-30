/***************************************************************************
                          addconstraintteachermingapsbetweenorderedpairofactivitytagsform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H
#define ADDCONSTRAINTTEACHERMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H

#include "ui_addconstraintteachermingapsbetweenorderedpairofactivitytagsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm : public QDialog, Ui::AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm(QWidget* parent);
	~AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm();
	
	void updateTeacherComboBox();
	void updateFirstActivityTagComboBox();
	void updateSecondActivityTagComboBox();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();

	void swap();
};

#endif
