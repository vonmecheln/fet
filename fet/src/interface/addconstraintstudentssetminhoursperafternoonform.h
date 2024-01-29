/***************************************************************************
                          addconstraintstudentssetminhoursperafternoonform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTSTUDENTSSETMINHOURSPERAFTERNOONFORM_H
#define ADDCONSTRAINTSTUDENTSSETMINHOURSPERAFTERNOONFORM_H

#include "ui_addconstraintstudentssetminhoursperafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMinHoursPerAfternoonForm : public QDialog, Ui::AddConstraintStudentsSetMinHoursPerAfternoonForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMinHoursPerAfternoonForm(QWidget* parent);
	~AddConstraintStudentsSetMinHoursPerAfternoonForm();
	
	void updateStudentsSetComboBox();

public slots:
	void addCurrentConstraint();
	
	void allowEmptyAfternoonsCheckBox_toggled();
};

#endif
