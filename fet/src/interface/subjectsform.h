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

#ifndef SUBJECTSFORM_H
#define SUBJECTSFORM_H

#include "ui_subjectsform_template.h"

class SubjectsForm : public QDialog, Ui::SubjectsForm_template
{
	Q_OBJECT
public:
	SubjectsForm(QWidget* parent);

	~SubjectsForm();

public Q_SLOTS:
	void addSubject();
	void removeSubject();
	void renameSubject();
	
	void moveSubjectUp();
	void moveSubjectDown();
	
	void sortSubjects();
	
	void subjectChanged(int index);
	
	void activateSubject();
	void deactivateSubject();
	
	void longName();
	void code();
	void comments();
};

#endif
