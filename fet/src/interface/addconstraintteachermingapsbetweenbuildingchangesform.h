/***************************************************************************
                          addconstraintteachermingapsbetweenbuildingchangesform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#ifndef ADDCONSTRAINTTEACHERMINGAPSBETWEENBUILDINGCHANGESFORM_H
#define ADDCONSTRAINTTEACHERMINGAPSBETWEENBUILDINGCHANGESFORM_H

#include "ui_addconstraintteachermingapsbetweenbuildingchangesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMinGapsBetweenBuildingChangesForm : public QDialog, Ui::AddConstraintTeacherMinGapsBetweenBuildingChangesForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMinGapsBetweenBuildingChangesForm(QWidget* parent);
	~AddConstraintTeacherMinGapsBetweenBuildingChangesForm();

	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
