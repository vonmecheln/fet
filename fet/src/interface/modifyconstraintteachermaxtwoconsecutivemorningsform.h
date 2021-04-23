/***************************************************************************
                          modifyconstraintteachermaxtwoconsecutivemorningsform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXTWOCONSECUTIVEMORNINGSFORM_H
#define MODIFYCONSTRAINTTEACHERMAXTWOCONSECUTIVEMORNINGSFORM_H

#include "ui_modifyconstraintteachermaxtwoconsecutivemorningsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxTwoConsecutiveMorningsForm : public QDialog, Ui::ModifyConstraintTeacherMaxTwoConsecutiveMorningsForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeacherMaxTwoConsecutiveMornings* _ctr;

	ModifyConstraintTeacherMaxTwoConsecutiveMorningsForm(QWidget* parent, ConstraintTeacherMaxTwoConsecutiveMornings* ctr);
	~ModifyConstraintTeacherMaxTwoConsecutiveMorningsForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
