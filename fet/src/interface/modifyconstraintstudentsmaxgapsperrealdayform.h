/***************************************************************************
                          modifyconstraintstudentsmaxgapsperrealdayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXGAPSPERREALDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXGAPSPERREALDAYFORM_H

#include "ui_modifyconstraintstudentsmaxgapsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxGapsPerRealDayForm : public QDialog, Ui::ModifyConstraintStudentsMaxGapsPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxGapsPerRealDay* _ctr;

	ModifyConstraintStudentsMaxGapsPerRealDayForm(QWidget* parent, ConstraintStudentsMaxGapsPerRealDay* ctr);
	~ModifyConstraintStudentsMaxGapsPerRealDayForm();

public slots:
	void ok();
	void cancel();
};

#endif
