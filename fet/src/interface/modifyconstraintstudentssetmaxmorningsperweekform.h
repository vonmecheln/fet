/***************************************************************************
                          modifyconstraintstudentssetmaxmorningsperweekform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXMORNINGSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXMORNINGSPERWEEKFORM_H

#include "ui_modifyconstraintstudentssetmaxmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxMorningsPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxMorningsPerWeek* _ctr;

	ModifyConstraintStudentsSetMaxMorningsPerWeekForm(QWidget* parent, ConstraintStudentsSetMaxMorningsPerWeek* ctr);
	~ModifyConstraintStudentsSetMaxMorningsPerWeekForm();

	void updateMaxMorningsSpinBox();
	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
