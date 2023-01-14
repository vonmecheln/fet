/***************************************************************************
                          modifyconstraintstudentssetmaxbuildingchangesperrealdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXBUILDINGCHANGESPERREALDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXBUILDINGCHANGESPERREALDAYFORM_H

#include "ui_modifyconstraintstudentssetmaxbuildingchangesperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxBuildingChangesPerRealDay* _ctr;

	ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayForm(QWidget* parent, ConstraintStudentsSetMaxBuildingChangesPerRealDay* ctr);
	~ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
