/***************************************************************************
                          addconstraintstudentssetmaxafternoonsperweekform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETMAXAFTERNOONSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXAFTERNOONSPERWEEKFORM_H

#include "ui_addconstraintstudentssetmaxafternoonsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxAfternoonsPerWeekForm : public QDialog, Ui::AddConstraintStudentsSetMaxAfternoonsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxAfternoonsPerWeekForm(QWidget* parent);
	~AddConstraintStudentsSetMaxAfternoonsPerWeekForm();

	void updateMaxAfternoonsSpinBox();
	void updateStudentsComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
