/***************************************************************************
                          modifyconstraintteacherminhoursdailyform.h  -  description
                             -------------------
    begin                : Sept 21, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERMINHOURSDAILYFORM_H
#define MODIFYCONSTRAINTTEACHERMINHOURSDAILYFORM_H

#include "modifyconstraintteacherminhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class ModifyConstraintTeacherMinHoursDailyForm : public ModifyConstraintTeacherMinHoursDailyForm_template  {
public:
	ConstraintTeacherMinHoursDaily* _ctr;

	ModifyConstraintTeacherMinHoursDailyForm(ConstraintTeacherMinHoursDaily* ctr);
	~ModifyConstraintTeacherMinHoursDailyForm();

	void updateMinHoursSpinBox();

	void constraintChanged();
	void ok();
	void cancel();
};

#endif