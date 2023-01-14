/***************************************************************************
                          modifyconstraintstudentssetmaxgapsperrealdayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTSTUDENTSSETMAXGAPSPERREALDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMAXGAPSPERREALDAYFORM_H

#include "ui_modifyconstraintstudentssetmaxgapsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMaxGapsPerRealDayForm : public QDialog, Ui::ModifyConstraintStudentsSetMaxGapsPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMaxGapsPerRealDay* _ctr;

	ModifyConstraintStudentsSetMaxGapsPerRealDayForm(QWidget* parent, ConstraintStudentsSetMaxGapsPerRealDay* ctr);
	~ModifyConstraintStudentsSetMaxGapsPerRealDayForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
