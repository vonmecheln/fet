/***************************************************************************
                          modifyconstraintstudentssetminrestinghoursform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMINRESTINGHOURSFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMINRESTINGHOURSFORM_H

#include "ui_modifyconstraintstudentssetminrestinghoursform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMinRestingHoursForm : public QDialog, Ui::ModifyConstraintStudentsSetMinRestingHoursForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMinRestingHours* _ctr;

	ModifyConstraintStudentsSetMinRestingHoursForm(QWidget* parent, ConstraintStudentsSetMinRestingHours* ctr);
	~ModifyConstraintStudentsSetMinRestingHoursForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
