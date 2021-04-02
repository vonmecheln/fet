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
#ifndef GROUPSFORM_H
#define GROUPSFORM_H

#include "groupsform_template.h"

class GroupsForm : public GroupsForm_template
{
public:
	GroupsForm();
	~GroupsForm();

	void addGroup();
	void removeGroup();
	void modifyGroup();
	void sortGroups();
	void yearChanged(const QString &yearName);
	void groupChanged(const QString &groupName);
	
	void activateStudents();
	void deactivateStudents();
};

#endif
