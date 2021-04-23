/***************************************************************************
                          addconstraintteacherminrestinghoursbetweenmorningandafternoonform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTTEACHERMINRESTINGHOURSBETWEENMORNINGANDAFTERNOONFORM_H
#define ADDCONSTRAINTTEACHERMINRESTINGHOURSBETWEENMORNINGANDAFTERNOONFORM_H

#include "ui_addconstraintteacherminrestinghoursbetweenmorningandafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm : public QDialog, Ui::AddConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm_template  {
	Q_OBJECT
public:
	AddConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm(QWidget* parent);
	~AddConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm();

public slots:
	void addCurrentConstraint();
};

#endif
