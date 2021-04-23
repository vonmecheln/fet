/***************************************************************************
                          modifyconstraintstudentsmaxmorningsperweekform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXMORNINGSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXMORNINGSPERWEEKFORM_H

#include "ui_modifyconstraintstudentsmaxmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxMorningsPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsMaxMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxMorningsPerWeek* _ctr;

	ModifyConstraintStudentsMaxMorningsPerWeekForm(QWidget* parent, ConstraintStudentsMaxMorningsPerWeek* ctr);
	~ModifyConstraintStudentsMaxMorningsPerWeekForm();

	void updateMaxMorningsSpinBox();

public slots:
	void ok();
	void cancel();
};

#endif
