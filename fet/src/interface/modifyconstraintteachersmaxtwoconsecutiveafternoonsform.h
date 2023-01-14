/***************************************************************************
                          modifyconstraintteachersmaxtwoconsecutiveafternoonsform.h  -  description
                             -------------------
    begin                : Oct 15, 2015
    copyright            : (C) 2005 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERSMAXTWOCONSECUTIVEAFTERNOONSFORM_H
#define MODIFYCONSTRAINTTEACHERSMAXTWOCONSECUTIVEAFTERNOONSFORM_H

#include "ui_modifyconstraintteachersmaxtwoconsecutiveafternoonsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMaxTwoConsecutiveAfternoonsForm : public QDialog, Ui::ModifyConstraintTeachersMaxTwoConsecutiveAfternoonsForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeachersMaxTwoConsecutiveAfternoons* _ctr;

	ModifyConstraintTeachersMaxTwoConsecutiveAfternoonsForm(QWidget* parent, ConstraintTeachersMaxTwoConsecutiveAfternoons* ctr);
	~ModifyConstraintTeachersMaxTwoConsecutiveAfternoonsForm();

public slots:
	void ok();
	void cancel();
};

#endif
