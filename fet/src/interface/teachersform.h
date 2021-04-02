//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TEACHERSFORM_H
#define TEACHERSFORM_H

#include "ui_teachersform_template.h"

class TeachersForm : public QDialog, Ui::TeachersForm_template
{
	Q_OBJECT
public:
	TeachersForm();

	~TeachersForm();

public slots:
	void addTeacher();
	void removeTeacher();
	void renameTeacher();
	void sortTeachers();
	
	void teacherChanged(int index);
	
	void activateTeacher();
	void deactivateTeacher();
};

#endif
