/***************************************************************************
                          modifyconstraintstudentsmaxgapsperdayform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXGAPSPERDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXGAPSPERDAYFORM_H

#include "ui_modifyconstraintstudentsmaxgapsperdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxGapsPerDayForm : public QDialog, Ui::ModifyConstraintStudentsMaxGapsPerDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxGapsPerDay* _ctr;

	ModifyConstraintStudentsMaxGapsPerDayForm(QWidget* parent, ConstraintStudentsMaxGapsPerDay* ctr);
	~ModifyConstraintStudentsMaxGapsPerDayForm();

public slots:
	void ok();
	void cancel();
};

#endif
