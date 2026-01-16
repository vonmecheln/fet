//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TEACHERSFORM_H
#define TEACHERSFORM_H

#include "ui_teachersform_template.h"

#include <QString>
#include <QHash>

class TeachersForm : public QDialog, Ui::TeachersForm_template
{
	Q_OBJECT
	
	QHash<QString, int> activeHoursHash;
	
public:
	TeachersForm(QWidget* parent);

	~TeachersForm();

public Q_SLOTS:
	void addTeacher();
	void removeTeacher();
	void modifyTeacher();
	
	void targetNumberOfHours();
	void qualifiedSubjects();

	void moveTeacherUp();
	void moveTeacherDown();

	void sortTeachers();
	
	void teacherChanged(int index);
	
	void activateTeacher();
	void deactivateTeacher();
	
	void longName();
	void code();
	void comments();
};

#endif
