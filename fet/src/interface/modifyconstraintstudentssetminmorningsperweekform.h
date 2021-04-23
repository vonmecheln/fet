/***************************************************************************
                          modifyconstraintstudentssetminmorningsperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMINMORNINGSPERWEEKFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMINMORNINGSPERWEEKFORM_H

#include "ui_modifyconstraintstudentssetminmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMinMorningsPerWeekForm : public QDialog, Ui::ModifyConstraintStudentsSetMinMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMinMorningsPerWeek* _ctr;

	ModifyConstraintStudentsSetMinMorningsPerWeekForm(QWidget* parent, ConstraintStudentsSetMinMorningsPerWeek* ctr);
	~ModifyConstraintStudentsSetMinMorningsPerWeekForm();

	void updateMinMorningsSpinBox();
	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
