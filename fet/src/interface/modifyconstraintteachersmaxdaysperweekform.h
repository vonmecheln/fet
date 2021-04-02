/***************************************************************************
                          modifyconstraintteachersmaxdaysperweekform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYCONSTRAINTTEACHERSMAXDAYSPERWEEKFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXDAYSPERWEEKFORM_H

#include "ui_modifyconstraintteachersmaxdaysperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"

#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>

class ModifyConstraintTeachersMaxDaysPerWeekForm : public QDialog, Ui::ModifyConstraintTeachersMaxDaysPerWeekForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMaxDaysPerWeek* _ctr;

	ModifyConstraintTeachersMaxDaysPerWeekForm(ConstraintTeachersMaxDaysPerWeek* ctr);
	~ModifyConstraintTeachersMaxDaysPerWeekForm();

	void updateMaxDaysSpinBox();
	//void updateTeachersComboBox();

public slots:
	void constraintChanged();
	void ok();
	void cancel();
};

#endif
