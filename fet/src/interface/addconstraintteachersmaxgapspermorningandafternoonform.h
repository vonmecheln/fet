/***************************************************************************
                          addconstraintteachersmaxgapspermorningandafternoonform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTTEACHERSMAXGAPSPERMORNINGANDAFTERNOONFORM_H
#define ADDCONSTRAINTTEACHERSMAXGAPSPERMORNINGANDAFTERNOONFORM_H

#include "ui_addconstraintteachersmaxgapspermorningandafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeachersMaxGapsPerMorningAndAfternoonForm : public QDialog, Ui::AddConstraintTeachersMaxGapsPerMorningAndAfternoonForm_template  {
	Q_OBJECT
public:
	AddConstraintTeachersMaxGapsPerMorningAndAfternoonForm(QWidget* parent);
	~AddConstraintTeachersMaxGapsPerMorningAndAfternoonForm();

public slots:
	void addCurrentConstraint();
	void addCurrentConstraints();
};

#endif
