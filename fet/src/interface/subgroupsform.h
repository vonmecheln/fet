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
#ifndef SUBGROUPSFORM_H
#define SUBGROUPSFORM_H

#include "subgroupsform_template.h"

class SubgroupsForm : public SubgroupsForm_template
{
public:
	SubgroupsForm();
	~SubgroupsForm();

	void addSubgroup();
	void removeSubgroup();
	void modifySubgroup();
	void sortSubgroups();
	void yearChanged(const QString &yearName);
	void groupChanged(const QString &groupName);
	void subgroupChanged(const QString &subgroupName);
	
	void activateStudents();
	void deactivateStudents();
};

#endif
