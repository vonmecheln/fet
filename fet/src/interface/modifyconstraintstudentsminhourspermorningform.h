/***************************************************************************
                          modifyconstraintstudentsminhourspermorningform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMINHOURSPERMORNINGFORM_H
#define MODIFYCONSTRAINTSTUDENTSMINHOURSPERMORNINGFORM_H

#include "ui_modifyconstraintstudentsminhourspermorningform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMinHoursPerMorningForm : public QDialog, Ui::ModifyConstraintStudentsMinHoursPerMorningForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMinHoursPerMorning* _ctr;

	ModifyConstraintStudentsMinHoursPerMorningForm(QWidget* parent, ConstraintStudentsMinHoursPerMorning* ctr);
	~ModifyConstraintStudentsMinHoursPerMorningForm();

public slots:
	void ok();
	void cancel();
};

#endif
