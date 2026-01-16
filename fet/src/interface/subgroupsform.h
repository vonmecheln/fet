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

#ifndef SUBGROUPSFORM_H
#define SUBGROUPSFORM_H

#include "ui_subgroupsform_template.h"

class SubgroupsForm : public QDialog, Ui::SubgroupsForm_template
{
	Q_OBJECT
public:
	SubgroupsForm(QWidget* parent);
	~SubgroupsForm();

public Q_SLOTS:
	void addSubgroup();
	void addExistingSubgroups();
	void removeSubgroup();
	void purgeSubgroup();
	void modifySubgroup();

	void moveSubgroupUp();
	void moveSubgroupDown();

	void sortSubgroups();
	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	void subgroupChanged(const QString& subgroupName);
	
	void activateStudents();
	void deactivateStudents();
	
	void longName();
	void code();
	void comments();
};

#endif
