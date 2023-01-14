/***************************************************************************
                          modifyconstraintteachermaxhoursperallafternoonsform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERMAXHOURSPERALLAFTERNOONSFORM_H
#define MODIFYCONSTRAINTTEACHERMAXHOURSPERALLAFTERNOONSFORM_H

#include "ui_modifyconstraintteachermaxhoursperallafternoonsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxHoursPerAllAfternoonsForm : public QDialog, Ui::ModifyConstraintTeacherMaxHoursPerAllAfternoonsForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxHoursPerAllAfternoons* _ctr;

	ModifyConstraintTeacherMaxHoursPerAllAfternoonsForm(QWidget* parent, ConstraintTeacherMaxHoursPerAllAfternoons* ctr);
	~ModifyConstraintTeacherMaxHoursPerAllAfternoonsForm();

	void updateMaxHoursSpinBox();

public slots:
	void ok();
	void cancel();
};

#endif
