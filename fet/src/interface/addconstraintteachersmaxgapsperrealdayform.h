/***************************************************************************
                          addconstraintteachersmaxgapsperrealdayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERSMAXGAPSPERREALDAYFORM_H
#define ADDCONSTRAINTTEACHERSMAXGAPSPERREALDAYFORM_H

#include "ui_addconstraintteachersmaxgapsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxGapsPerRealDayForm : public QDialog, Ui::AddConstraintTeachersMaxGapsPerRealDayForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxGapsPerRealDayForm(QWidget* parent);
	~AddConstraintTeachersMaxGapsPerRealDayForm();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
