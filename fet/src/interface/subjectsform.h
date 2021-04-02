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
#ifndef SUBJECTSFORM_H
#define SUBJECTSFORM_H

#include "ui_subjectsform_template.h"

class SubjectsForm : public QDialog, Ui::SubjectsForm_template
{
	Q_OBJECT
public:
	SubjectsForm();

	~SubjectsForm();

public slots:
	void addSubject();
	void removeSubject();
	void renameSubject();
	void sortSubjects();
	
	void subjectChanged(int index);
	
	void activateSubject();
	void deactivateSubject();
};

#endif
