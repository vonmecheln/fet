/***************************************************************************
                          modifyconstraintteachermaxtwoconsecutiveafternoonsform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXTWOCONSECUTIVEAFTERNOONSFORM_H
#define MODIFYCONSTRAINTTEACHERMAXTWOCONSECUTIVEAFTERNOONSFORM_H

#include "ui_modifyconstraintteachermaxtwoconsecutiveafternoonsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm : public QDialog, Ui::ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm_template  {
	Q_OBJECT
	
public:
	ConstraintTeacherMaxTwoConsecutiveAfternoons* _ctr;

	ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm(QWidget* parent, ConstraintTeacherMaxTwoConsecutiveAfternoons* ctr);
	~ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm();

	void updateTeachersComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
