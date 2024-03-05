/***************************************************************************
                          modifyconstraintstudentsmaxbuildingchangesperdayinintervalform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXBUILDINGCHANGESPERDAYININTERVALFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXBUILDINGCHANGESPERDAYININTERVALFORM_H

#include "ui_modifyconstraintstudentsmaxbuildingchangesperdayinintervalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxBuildingChangesPerDayInIntervalForm : public QDialog, Ui::ModifyConstraintStudentsMaxBuildingChangesPerDayInIntervalForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxBuildingChangesPerDayInInterval* _ctr;

	ModifyConstraintStudentsMaxBuildingChangesPerDayInIntervalForm(QWidget* parent, ConstraintStudentsMaxBuildingChangesPerDayInInterval* ctr);
	~ModifyConstraintStudentsMaxBuildingChangesPerDayInIntervalForm();

public slots:
	void ok();
	void cancel();
};

#endif
