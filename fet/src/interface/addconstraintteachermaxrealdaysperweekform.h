/***************************************************************************
                          addconstraintteachermaxrealdaysperweekform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERMAXREALDAYSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERMAXREALDAYSPERWEEKFORM_H

#include "ui_addconstraintteachermaxrealdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxRealDaysPerWeekForm : public QDialog, Ui::AddConstraintTeacherMaxRealDaysPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxRealDaysPerWeekForm(QWidget* parent);
	~AddConstraintTeacherMaxRealDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
