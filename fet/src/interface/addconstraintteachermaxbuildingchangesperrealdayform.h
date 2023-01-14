/***************************************************************************
                          addconstraintteachermaxbuildingchangesperrealdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERREALDAYFORM_H
#define ADDCONSTRAINTTEACHERMAXBUILDINGCHANGESPERREALDAYFORM_H

#include "ui_addconstraintteachermaxbuildingchangesperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxBuildingChangesPerRealDayForm : public QDialog, Ui::AddConstraintTeacherMaxBuildingChangesPerRealDayForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxBuildingChangesPerRealDayForm(QWidget* parent);
	~AddConstraintTeacherMaxBuildingChangesPerRealDayForm();

	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
