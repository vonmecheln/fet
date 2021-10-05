/***************************************************************************
                          modifyconstraintteachermaxthreeconsecutivedaysform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERMAXTHREECONSECUTIVEDAYSFORM_H
#define MODIFYCONSTRAINTTEACHERMAXTHREECONSECUTIVEDAYSFORM_H

#include "ui_modifyconstraintteachermaxthreeconsecutivedaysform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxThreeConsecutiveDaysForm : public QDialog, Ui::ModifyConstraintTeacherMaxThreeConsecutiveDaysForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeacherMaxThreeConsecutiveDays* _ctr;

	ModifyConstraintTeacherMaxThreeConsecutiveDaysForm(QWidget* parent, ConstraintTeacherMaxThreeConsecutiveDays* ctr);
	~ModifyConstraintTeacherMaxThreeConsecutiveDaysForm();

	void updateExceptionCheckBox();
	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
