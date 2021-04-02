/***************************************************************************
                          modifyconstraintteachersminhoursdailyform.h  -  description
                             -------------------
    begin                : July 22, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINHOURSDAILYFORM_H
#define MODIFYCONSTRAINTTEACHERSMINHOURSDAILYFORM_H

#include "modifyconstraintteachersminhoursdailyform_template.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"
#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class ModifyConstraintTeachersMinHoursDailyForm : public ModifyConstraintTeachersMinHoursDailyForm_template  {
public:
	ConstraintTeachersMinHoursDaily* _ctr;

	ModifyConstraintTeachersMinHoursDailyForm(ConstraintTeachersMinHoursDaily* ctr);
	~ModifyConstraintTeachersMinHoursDailyForm();

	void updateMinHoursSpinBox();

	void constraintChanged();
	void ok();
	void cancel();
};

#endif
