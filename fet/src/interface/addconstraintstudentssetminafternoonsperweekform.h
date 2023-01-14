/***************************************************************************
                          addconstraintstudentssetminafternoonsperweekform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETMINAFTERNOONSPERWEEKFORM_H
#define ADDCONSTRAINTSTUDENTSSETMINAFTERNOONSPERWEEKFORM_H

#include "ui_addconstraintstudentssetminafternoonsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMinAfternoonsPerWeekForm : public QDialog, Ui::AddConstraintStudentsSetMinAfternoonsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMinAfternoonsPerWeekForm(QWidget* parent);
	~AddConstraintStudentsSetMinAfternoonsPerWeekForm();

	void updateMinAfternoonsSpinBox();
	void updateStudentsComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
