/***************************************************************************
                          addconstraintteachermaxhoursdailyform.h  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERMAXHOURSDAILYFORM_H
#define ADDCONSTRAINTTEACHERMAXHOURSDAILYFORM_H

#include "ui_addconstraintteachermaxhoursdailyform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMaxHoursDailyForm : public QDialog, Ui::AddConstraintTeacherMaxHoursDailyForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMaxHoursDailyForm(QWidget* parent);
	~AddConstraintTeacherMaxHoursDailyForm();

	void updateMaxHoursSpinBox();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
