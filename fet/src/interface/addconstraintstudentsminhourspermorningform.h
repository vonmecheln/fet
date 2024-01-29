/***************************************************************************
                          addconstraintstudentsminhourspermorningform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMINHOURSPERMORNINGFORM_H
#define ADDCONSTRAINTSTUDENTSMINHOURSPERMORNINGFORM_H

#include "ui_addconstraintstudentsminhourspermorningform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMinHoursPerMorningForm : public QDialog, Ui::AddConstraintStudentsMinHoursPerMorningForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMinHoursPerMorningForm(QWidget* parent);
	~AddConstraintStudentsMinHoursPerMorningForm();

public slots:
	void addCurrentConstraint();
	
	void allowEmptyMorningsCheckBox_toggled();
};

#endif
