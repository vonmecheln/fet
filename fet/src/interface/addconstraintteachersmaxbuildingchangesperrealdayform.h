/***************************************************************************
                          addconstraintteachersmaxbuildingchangesperrealdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTTEACHERSMAXBUILDINGCHANGESPERREALDAYFORM_H
#define ADDCONSTRAINTTEACHERSMAXBUILDINGCHANGESPERREALDAYFORM_H

#include "ui_addconstraintteachersmaxbuildingchangesperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxBuildingChangesPerRealDayForm : public QDialog, Ui::AddConstraintTeachersMaxBuildingChangesPerRealDayForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxBuildingChangesPerRealDayForm(QWidget* parent);
	~AddConstraintTeachersMaxBuildingChangesPerRealDayForm();

public slots:
	void addCurrentConstraint();
};

#endif
