/***************************************************************************
                          modifyconstraintteachersminafternoonsperweekform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMINAFTERNOONSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMINAFTERNOONSPERWEEKFORM_H

#include "ui_modifyconstraintteachersminafternoonsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinAfternoonsPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMinAfternoonsPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMinAfternoonsPerWeek* _ctr;

	ModifyConstraintTeachersMinAfternoonsPerWeekForm(QWidget* parent, ConstraintTeachersMinAfternoonsPerWeek* ctr);
	~ModifyConstraintTeachersMinAfternoonsPerWeekForm();

	void updateMinAfternoonsSpinBox();
	//void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
