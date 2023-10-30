/***************************************************************************
                          addconstraintteachermaxthreeconsecutivedaysform.h  -  description
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

#ifndef ADDCONSTRAINTTEACHERMAXTHREECONSECUTIVEDAYSFORM_H
#define ADDCONSTRAINTTEACHERMAXTHREECONSECUTIVEDAYSFORM_H

#include "ui_addconstraintteachermaxthreeconsecutivedaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxThreeConsecutiveDaysForm : public QDialog, Ui::AddConstraintTeacherMaxThreeConsecutiveDaysForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxThreeConsecutiveDaysForm(QWidget* parent);
	~AddConstraintTeacherMaxThreeConsecutiveDaysForm();

	void updateExceptionCheckBox();
	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
