/***************************************************************************
                          addconstraintstudentsminmorningsperweekform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMINMORNINGSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSMINMORNINGSPERWEEKFORM_H

#include "ui_addconstraintstudentsminmorningsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMinMorningsPerWeekForm : public QDialog, Ui::AddConstraintStudentsMinMorningsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMinMorningsPerWeekForm(QWidget* parent);
	~AddConstraintStudentsMinMorningsPerWeekForm();

	void updateMinMorningsSpinBox();

public slots:
	void addCurrentConstraint();
};

#endif
