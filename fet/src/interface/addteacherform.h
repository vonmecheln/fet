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
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDTEACHERFORM_H
#define ADDTEACHERFORM_H

#include "teacher.h"

#include "ui_addteacherform_template.h"

class AddTeacherForm : public QDialog, Ui::AddTeacherForm_template
{
	Q_OBJECT
	
	bool teachersMorningsAfternoonsBehaviorChosen;
	
public:
	AddTeacherForm(QWidget* parent);

	~AddTeacherForm();

public slots:
	void addTeacher();
	void help();
};

#endif
