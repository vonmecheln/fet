/***************************************************************************
                          addconstraintteachersminhoursperafternoonform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERSMINHOURSPERAFTERNOONFORM_H
#define ADDCONSTRAINTTEACHERSMINHOURSPERAFTERNOONFORM_H

#include "ui_addconstraintteachersminhoursperafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMinHoursPerAfternoonForm : public QDialog, Ui::AddConstraintTeachersMinHoursPerAfternoonForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMinHoursPerAfternoonForm(QWidget* parent);
	~AddConstraintTeachersMinHoursPerAfternoonForm();

	void updateMinHoursSpinBox();

public slots:
	void addCurrentConstraint();

	void on_allowEmptyAfternoonsCheckBox_toggled();
};

#endif
