/***************************************************************************
                          addconstraintstudentssetmaxthreeconsecutivedaysform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETMAXTHREECONSECUTIVEDAYSFORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXTHREECONSECUTIVEDAYSFORM_H

#include "ui_addconstraintstudentssetmaxthreeconsecutivedaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxThreeConsecutiveDaysForm : public QDialog, Ui::AddConstraintStudentsSetMaxThreeConsecutiveDaysForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxThreeConsecutiveDaysForm(QWidget* parent);
	~AddConstraintStudentsSetMaxThreeConsecutiveDaysForm();

	void updateExceptionCheckBox();
	void updateStudentsComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
