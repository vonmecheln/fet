/***************************************************************************
                          modifyconstraintteachermaxgapsperweekforrealdaysform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXGAPSPERWEEKFORREALDAYSFORM_H
#define MODIFYCONSTRAINTTEACHERMAXGAPSPERWEEKFORREALDAYSFORM_H

#include "ui_modifyconstraintteachermaxgapsperweekforrealdaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxGapsPerWeekForRealDaysForm : public QDialog, Ui::ModifyConstraintTeacherMaxGapsPerWeekForRealDaysForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxGapsPerWeekForRealDays* _ctr;

	ModifyConstraintTeacherMaxGapsPerWeekForRealDaysForm(QWidget* parent, ConstraintTeacherMaxGapsPerWeekForRealDays* ctr);
	~ModifyConstraintTeacherMaxGapsPerWeekForRealDaysForm();

public slots:
	void ok();
	void cancel();
};

#endif
