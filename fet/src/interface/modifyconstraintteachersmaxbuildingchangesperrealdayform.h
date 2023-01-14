/***************************************************************************
                          modifyconstraintteachersmaxbuildingchangesperrealdayform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXBUILDINGCHANGESPERREALDAYFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXBUILDINGCHANGESPERREALDAYFORM_H

#include "ui_modifyconstraintteachersmaxbuildingchangesperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxBuildingChangesPerRealDayForm : public QDialog, Ui::ModifyConstraintTeachersMaxBuildingChangesPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMaxBuildingChangesPerRealDay* _ctr;

	ModifyConstraintTeachersMaxBuildingChangesPerRealDayForm(QWidget* parent, ConstraintTeachersMaxBuildingChangesPerRealDay* ctr);
	~ModifyConstraintTeachersMaxBuildingChangesPerRealDayForm();

public slots:
	void ok();
	void cancel();
};

#endif
