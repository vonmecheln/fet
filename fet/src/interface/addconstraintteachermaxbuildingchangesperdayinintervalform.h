/***************************************************************************
                          addconstraintteachermaxbuildingchangesperdayinintervalform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERDAYININTERVALFORM_H
#define ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERDAYININTERVALFORM_H

#include "ui_addconstraintteachermaxbuildingchangesperdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxBuildingChangesPerDayInIntervalForm : public QDialog, Ui::AddConstraintTeacherMaxBuildingChangesPerDayInIntervalForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxBuildingChangesPerDayInIntervalForm(QWidget* parent);
	~AddConstraintTeacherMaxBuildingChangesPerDayInIntervalForm();

	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
