/***************************************************************************
                          modifyconstraintstudentsminhoursperafternoonform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMINHOURSPERAFTERNOONFORM_H
#define MODIFYCONSTRAINTSTUDENTSMINHOURSPERAFTERNOONFORM_H

#include "ui_modifyconstraintstudentsminhoursperafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMinHoursPerAfternoonForm : public QDialog, Ui::ModifyConstraintStudentsMinHoursPerAfternoonForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMinHoursPerAfternoon* _ctr;

	ModifyConstraintStudentsMinHoursPerAfternoonForm(QWidget* parent, ConstraintStudentsMinHoursPerAfternoon* ctr);
	~ModifyConstraintStudentsMinHoursPerAfternoonForm();

public slots:
	void ok();
	void cancel();
};

#endif
