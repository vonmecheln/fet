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

#ifndef GROUPSFORM_H
#define GROUPSFORM_H

#include "ui_groupsform_template.h"

class GroupsForm : public QDialog, Ui::GroupsForm_template
{
	Q_OBJECT
public:
	GroupsForm(QWidget* parent);
	~GroupsForm();

public Q_SLOTS:
	void addGroup();
	void addExistingGroups();
	void removeGroup();
	void purgeGroup();
	void modifyGroup();

	void moveGroupUp();
	void moveGroupDown();
	
	void sortGroups();
	
	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	
	void activateStudents();
	void deactivateStudents();
	
	void longName();
	void code();
	void comments();
};

#endif
