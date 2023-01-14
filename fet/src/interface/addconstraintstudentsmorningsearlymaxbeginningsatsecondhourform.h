/***************************************************************************
                          addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSMORNINGSEARLYMAXBEGINNINGSATSECONDHOURFORM_H
#define ADDCONSTRAINTSTUDENTSMORNINGSEARLYMAXBEGINNINGSATSECONDHOURFORM_H

#include "ui_addconstraintstudentsmorningsearlymaxbeginningsatsecondhourform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm : public QDialog, Ui::AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm(QWidget* parent);
	~AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm();

public slots:
	void addCurrentConstraint();
};

#endif
