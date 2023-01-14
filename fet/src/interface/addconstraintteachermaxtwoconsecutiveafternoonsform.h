/***************************************************************************
                          addconstraintteachermaxtwoconsecutiveafternoonsform.h  -  description
                             -------------------
    begin                : Oct 15, 2015
    copyright            : (C) 2015 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERMAXTWOCONSECUTIVEAFTERNOONSFORM_H
#define ADDCONSTRAINTTEACHERMAXTWOCONSECUTIVEAFTERNOONSFORM_H

#include "ui_addconstraintteachermaxtwoconsecutiveafternoonsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxTwoConsecutiveAfternoonsForm : public QDialog, Ui::AddConstraintTeacherMaxTwoConsecutiveAfternoonsForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxTwoConsecutiveAfternoonsForm(QWidget* parent);
	~AddConstraintTeacherMaxTwoConsecutiveAfternoonsForm();

	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
