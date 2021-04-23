/***************************************************************************
                          addconstraintteachermaxafternoonsperweekform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#ifndef ADDCONSTRAINTTEACHERMAXAFTERNOONSPERWEEKFORM_H
#define ADDCONSTRAINTTEACHERMAXAFTERNOONSPERWEEKFORM_H

#include "ui_addconstraintteachermaxafternoonsperweekform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxAfternoonsPerWeekForm : public QDialog, Ui::AddConstraintTeacherMaxAfternoonsPerWeekForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxAfternoonsPerWeekForm(QWidget* parent);
	~AddConstraintTeacherMaxAfternoonsPerWeekForm();

	void updateMaxAfternoonsSpinBox();
	void updateTeachersComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
