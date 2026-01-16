//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDTEACHERFORM_H
#define ADDTEACHERFORM_H

#include "teacher.h"

#include "ui_addteacherform_template.h"

#include <QHash>
#include <QString>

class AddTeacherForm : public QDialog, Ui::AddTeacherForm_template
{
	Q_OBJECT
	
	bool teachersMorningsAfternoonsBehaviorChosen;
	
	QHash<QString, int>* activeHoursHashPointer;
	
public:
	AddTeacherForm(QWidget* parent, QHash<QString, int>& activeHoursHash);

	~AddTeacherForm();

public Q_SLOTS:
	void addTeacher();
	void help();
};

#endif
