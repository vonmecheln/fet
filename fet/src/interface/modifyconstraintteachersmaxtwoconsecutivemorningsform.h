/***************************************************************************
                          modifyconstraintteachersmaxtwoconsecutivemorningsform.h  -  description
                             -------------------
    begin                : Oct 15, 2015
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXTWOCONSECUTIVEMORNINGSFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXTWOCONSECUTIVEMORNINGSFORM_H

#include "ui_modifyconstraintteachersmaxtwoconsecutivemorningsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxTwoConsecutiveMorningsForm : public QDialog, Ui::ModifyConstraintTeachersMaxTwoConsecutiveMorningsForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMaxTwoConsecutiveMornings* _ctr;

	ModifyConstraintTeachersMaxTwoConsecutiveMorningsForm(QWidget* parent, ConstraintTeachersMaxTwoConsecutiveMornings* ctr);
	~ModifyConstraintTeachersMaxTwoConsecutiveMorningsForm();

public slots:
	void ok();
	void cancel();
};

#endif
